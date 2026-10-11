// prompt_polisher.cpp  (v2: long-form paragraph output, detail-fidelity check, self-test)
//
// Turns a raw, messy prompt into one polished, context-specific prompt written as
// complete paragraphs (each at least 4 lines long) that can be pasted into any AI chatbot.
// No external libraries and no API calls.
//
// Build :  g++ -std=c++17 -O2 prompt_polisher.cpp -o prompt_polisher
// Run   :  ./prompt_polisher            (interactive)
//          ./prompt_polisher --selftest (runs built-in verification suite)
//
// Input : type/paste the raw prompt, then finish with a line containing only END
//         (one-liners: just press Enter on an empty line). Type "exit" to quit.
// Output: shown on screen (wrapped) and saved to polished_prompt.txt (one line per paragraph).

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

static const int WRAP_WIDTH = 100;  // width used to measure and display paragraph lines
static const int MIN_LINES = 4;     // every prose paragraph must be at least this many lines

// ----------------------------------------------------------------------------
// String helpers
// ----------------------------------------------------------------------------
static string lower(string s) {
    for (auto &c : s) c = (char)tolower((unsigned char)c);
    return s;
}

static string trim(const string &s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

static string collapse(const string &s) {
    string o;
    bool sp = false;
    for (char c : s) {
        if (isspace((unsigned char)c)) {
            if (!sp && !o.empty()) o += ' ';
            sp = true;
        } else {
            o += c;
            sp = false;
        }
    }
    return trim(o);
}

static string replaceAll(string s, const string &from, const string &to) {
    size_t pos = 0;
    while ((pos = s.find(from, pos)) != string::npos) {
        s.replace(pos, from.size(), to);
        pos += to.size();
    }
    return s;
}

// Whole-word / whole-phrase match (works for "c++", "pros and cons", ...)
static bool has(const string &low, const string &phrase) {
    size_t pos = 0;
    while ((pos = low.find(phrase, pos)) != string::npos) {
        bool l = pos == 0 || !isalnum((unsigned char)low[pos - 1]);
        size_t e = pos + phrase.size();
        bool r = e >= low.size() || !isalnum((unsigned char)low[e]);
        if (l && r) return true;
        pos++;
    }
    return false;
}

static int hits(const string &low, const vector<string> &v) {
    int n = 0;
    for (auto &p : v)
        if (has(low, p)) n++;
    return n;
}

static int wordCount(const string &s) {
    istringstream in(s);
    string t;
    int n = 0;
    while (in >> t) n++;
    return n;
}

static string joinWith(const vector<string> &v, const string &sep) {
    string s;
    for (size_t i = 0; i < v.size(); ++i) s += (i ? sep : "") + v[i];
    return s;
}

static string article(const string &w) {
    return (!w.empty() && string("aeiouAEIOU").find(w[0]) != string::npos) ? "an " : "a ";
}

static vector<string> wrapLines(const string &p, int width) {
    vector<string> lines;
    istringstream in(p);
    string w, cur;
    while (in >> w) {
        if (cur.empty()) cur = w;
        else if (cur.size() + 1 + w.size() <= (size_t)width) cur += ' ' + w;
        else {
            lines.push_back(cur);
            cur = w;
        }
    }
    if (!cur.empty()) lines.push_back(cur);
    return lines;
}

// ----------------------------------------------------------------------------
// Intent catalogue (all prose fragments are written to be joined into paragraphs)
// ----------------------------------------------------------------------------
struct Intent {
    string name;
    vector<string> keys;
    int weight;
    string role;      // "{lang}" is replaced by the detected programming language
    string defTone;
    string mission;   // sentence fragment, no final period
    string desire;    // what the person ultimately wants
    string format;    // how the answer must be shaped, no final period
    vector<string> quality;   // clauses, joined with ordinals
    vector<string> approach;  // gerund phrases, joined as a sequence
};

static vector<Intent> intents() {
    return {
        {"coding",
         {"code", "program", "script", "function", "class", "algorithm", "implement", "build", "app",
          "website", "api", "library", "module", "software", "backend", "frontend", "database", "game",
          "bot", "automation", "cli", "project", "compile", "system", "application", "scraper",
          "scrape", "web", "extension", "plugin", "server"},
         1,
         "senior {lang} engineer with years of production experience",
         "precise, technical and free of fluff",
         "Your job is to design and deliver software that genuinely works, reasoning about the requirements the way a careful engineer would before writing a single line",
         "a complete, working piece of code they can compile or run immediately, together with enough explanation to understand and modify it, rather than a sketch, a fragment or a list of vague suggestions",
         "Deliver the complete code in properly fenced code blocks, in the right order if there are several files, and follow it with a clear explanation of how the program is structured, how it works, and exactly how to build and run it",
         {"the code must run exactly as written, with no placeholders, no pseudo-code and no omitted sections",
          "naming and structure must be clean, with comments only where they genuinely add understanding",
          "invalid input, edge cases and failure paths must be handled gracefully instead of being ignored",
          "the solution must follow the idioms and best practices of the language it is written in"},
         {"restating the requirements to yourself in one precise line so that nothing is misread",
          "choosing the simplest design that fully satisfies every stated requirement",
          "writing the complete implementation without skipping any part of it",
          "mentally running it against normal inputs and edge cases before you present it"}},

        {"debugging",
         {"bug", "error", "fix", "debug", "crash", "exception", "traceback", "not working", "broken",
          "fails", "failing", "issue", "segfault", "wrong output", "doesn't work", "doesnt work"},
         2,
         "expert {lang} debugger and meticulous code reviewer",
         "direct, diagnostic and calm",
         "Your job is to find the real reason something fails, not to guess at symptoms, by reading the code and any error output as carefully as a seasoned engineer would",
         "the exact root cause explained in plain words, a corrected version that actually works, and the confidence that the same problem will not quietly return",
         "Begin with the root cause in plain words, then give the corrected code in full so that it can be pasted straight in, and close with a short note on how to prevent the same class of problem in future",
         {"the diagnosis must identify the true root cause rather than a superficial symptom",
          "the fix must be minimal yet complete, shown in full wherever partial snippets could confuse",
          "any other bugs, risks or bad practices noticed along the way must be mentioned briefly"},
         {"reading the code and any error message slowly and completely",
          "locating the exact line or piece of logic responsible",
          "explaining precisely why it fails",
          "producing the fix and sanity-checking that it really resolves the problem"}},

        {"writing",
         {"write", "essay", "email", "letter", "article", "blog", "post", "story", "poem", "caption",
          "speech", "cover letter", "resume", "report", "message", "draft", "bio", "description",
          "linkedin", "newsletter", "copy", "slogan", "application"},
         1,
         "professional writer and exacting editor",
         "natural, human and polished",
         "Your job is to produce writing that reads as though a skilled person wrote it for this exact purpose and this exact reader, with a clear angle from the very first line",
         "a finished piece that is ready to send, post or publish without further editing, in a voice that sounds genuinely human rather than templated",
         "Deliver only the finished piece, ready to use, with no commentary wrapped around it, no alternative versions unless they were requested, and formatting that suits the place where it will be used",
         {"the writing must sound like a real person, with concrete detail and varied sentence rhythm instead of template phrasing",
          "clichés, empty openers and generic buzzwords must be avoided completely",
          "the voice must stay consistent from the first line to the last",
          "the result must be genuinely ready to send or publish without further editing"},
         {"identifying the purpose of the piece and exactly who will read it",
          "choosing the strongest angle and opening with it",
          "writing the full piece in one coherent voice",
          "cutting anything that does not earn its place"}},

        {"explanation",
         {"explain", "what is", "what are", "how does", "how do", "why", "teach", "understand", "learn",
          "eli5", "concept", "meaning", "difference between", "tell me about"},
         1,
         "patient expert teacher who can explain hard ideas simply",
         "clear, concrete and encouraging",
         "Your job is to make the subject genuinely understood, not merely described, by building the explanation from first principles and anchoring it in examples",
         "real understanding they can reuse and explain to someone else, rather than a definition they will forget within an hour",
         "Open with the core idea in a single sentence, then build the explanation in layers from the basics to the nuances, using at least one concrete example or analogy, and end with the most common misconception",
         {"accuracy must come first, and simplicity must never be bought at the price of correctness",
          "at least one concrete example or analogy must anchor the abstract ideas",
          "every technical term must be defined the first time it appears",
          "the explanation must finish with the most common misconception or pitfall"},
         {"stating the core idea in one clear sentence",
          "expanding step by step from the basics towards the nuances",
          "illustrating each key step with an example",
          "checking that nothing depends on unexplained jargon"}},

        {"analysis",
         {"analyze", "analyse", "evaluate", "review", "critique", "assess", "compare", "pros and cons",
          "versus", "vs", "better", "which is", "audit", "feedback", "worth it"},
         1,
         "sharp, impartial analyst who commits to conclusions",
         "objective and evidence-driven",
         "Your job is to weigh the options on their merits and reach a clear conclusion, reasoning from evidence rather than repeating vague both-sides summaries",
         "a decisive, well-reasoned recommendation they can act on, with the trade-offs made explicit so that they can disagree intelligently",
         "State the verdict first, then the reasoning behind it, then the trade-offs and the strongest counter-argument, using a table whenever several options are compared on the same criteria",
         {"a clear recommendation must be given and justified rather than hidden behind hedging",
          "every claim must rest on evidence or explicit reasoning, and anything that cannot be known must be stated plainly",
          "the strongest counter-argument must be addressed honestly",
          "vague both-sides filler must be avoided entirely"},
         {"defining the criteria that actually matter for this decision",
          "evaluating each option against those criteria",
          "weighing the trade-offs honestly",
          "concluding with a decisive recommendation"}},

        {"planning",
         {"plan", "roadmap", "schedule", "strategy", "itinerary", "routine", "workflow", "timeline",
          "checklist", "steps to", "how to start", "organize", "prepare", "goal", "study plan"},
         1,
         "experienced strategist and practical planner",
         "practical, decisive and realistic",
         "Your job is to turn the goal into a plan that a real person can start executing today, ordered by priority and dependency rather than listed at random",
         "a realistic, prioritised plan with concrete actions, sensible timings and clear checkpoints, so that they always know exactly what to do next",
         "Present a prioritised, time-ordered plan in which every step is a concrete action with a duration or deadline, grouped into phases, and finish with how progress will be measured",
         {"every step must be an action that can be started today, not a vague intention",
          "timings and dependencies must be realistic for one person's actual capacity",
          "priorities must be ruthless, with the most important work first",
          "the plan must include a way to measure progress and a fallback if it slips"},
         {"clarifying the end goal and the real constraints",
          "breaking the work into sensible phases",
          "ordering the actions by priority and dependency",
          "adding checkpoints and a fallback"}},

        {"summarizing",
         {"summarize", "summarise", "summary", "tldr", "shorten", "condense", "key points", "brief me",
          "recap"},
         2,
         "precise summarizer who never distorts the source",
         "neutral, compact and faithful",
         "Your job is to compress the material without distorting it, keeping every key fact and decision while removing repetition and decoration",
         "a faithful condensed version they can rely on instead of rereading the original",
         "Give only the summary in the requested shape, starting with the single most important point and followed by the supporting points in order of importance",
         {"every key fact, decision and number must be preserved",
          "nothing may be added that is not in the source",
          "the original meaning and emphasis must survive the compression"},
         {"identifying the main point and the supporting points",
          "compressing them without changing their meaning",
          "verifying that nothing important was lost"}},

        {"brainstorming",
         {"ideas", "brainstorm", "suggest", "names", "options", "recommend", "alternatives", "creative",
          "concepts", "inspiration"},
         1,
         "inventive creative strategist",
         "bold, fresh and specific",
         "Your job is to generate a wide spread of genuinely different ideas and then filter them hard, so that the person receives the strongest few rather than a pile",
         "original, specific ideas worth acting on, ranked clearly, with one standout recommendation",
         "Provide a ranked set of ideas, each with a one-line pitch and the reason it works, and mark the single best pick at the end with a short justification",
         {"the ideas must be clearly distinct from one another rather than variations of one theme",
          "they must be specific and original, avoiding the obvious answers anyone would give first",
          "they must be ranked, with the top pick justified"},
         {"generating a wide spread of different directions",
          "discarding the generic ones",
          "ranking the survivors and explaining the top choice"}},

        {"rewriting",
         {"translate", "rewrite", "rephrase", "proofread", "improve", "polish", "paraphrase", "grammar",
          "edit", "correct"},
         2,
         "expert editor and translator",
         "faithful to the original voice but sharper",
         "Your job is to improve or transform the text while preserving exactly what it means and who is speaking",
         "text that says the same thing more clearly and more naturally, in the author's own voice",
         "Return the final text first, then at most three short notes on the main changes made, and nothing else",
         {"the original meaning and intent must be preserved exactly",
          "grammar, flow and word choice must be fixed",
          "the author's own voice must be kept instead of being replaced by a generic one"},
         {"understanding what the text is trying to achieve",
          "reworking it for clarity and impact",
          "comparing it against the original to confirm the meaning has not shifted"}},

        {"math/data",
         {"calculate", "solve", "equation", "math", "statistics", "data", "formula", "excel",
          "spreadsheet", "probability", "derive", "proof", "integral"},
         1,
         "rigorous mathematician and careful data analyst",
         "exact, methodical and unhurried",
         "Your job is to solve the problem correctly and show the work in a way that can be checked line by line",
         "a correct final answer whose working they can follow and verify themselves",
         "State the final answer clearly, preceded by the working shown step by step, with assumptions and units written out explicitly",
         {"every step must be shown so the result can be checked",
          "assumptions and units must be stated explicitly",
          "the answer must be verified using a second method or a sanity check"},
         {"restating the problem formally",
          "solving it one verifiable step at a time",
          "verifying the answer independently"}},
    };
}

static Intent generalIntent() {
    return {"general",
            {},
            1,
            "knowledgeable, resourceful expert in whatever domain this request belongs to",
            "clear, direct and helpful",
            "Your job is to work out what the person actually needs and deliver exactly that, specific to their situation rather than generic",
            "a precise, directly usable answer that saves them from having to ask follow-up questions",
            "Give the answer first and then only the supporting detail that changes what the person will do next, organised so that it can be read quickly",
            {"the answer must be accurate and specific to this exact request",
             "anything that cannot be known must be stated plainly instead of being guessed",
             "nothing may be padded, and every sentence must earn its place"},
            {"working out what the person actually needs",
             "answering that directly",
             "adding only the context that changes their next step"}};
}

// ----------------------------------------------------------------------------
// Detection tables
// ----------------------------------------------------------------------------
struct Labelled {
    string label;
    vector<string> keys;
    string text;
};

static const vector<pair<string, vector<string>>> LANGS = {
    {"C++", {"c++", "cpp", "c plus plus"}},
    {"C", {"c program", "c language", "in c", "ansi c"}},
    {"Python", {"python", "django", "flask", "pandas", "numpy"}},
    {"Java", {"java", "spring boot"}},
    {"JavaScript", {"javascript", "js", "node", "nodejs", "react", "express", "typescript"}},
    {"HTML/CSS", {"html", "css", "tailwind"}},
    {"SQL", {"sql", "mysql", "postgres", "sqlite"}},
    {"Rust", {"rust"}},
    {"Go", {"golang"}},
    {"C#", {"c#", "csharp", ".net", "unity"}},
    {"Kotlin", {"kotlin"}},
    {"Swift", {"swift", "swiftui"}},
    {"PHP", {"php", "laravel"}},
    {"Bash", {"bash", "shell script"}},
};

static const vector<Labelled> TONES = {
    {"formal", {"formal", "professional", "official", "corporate", "polite"}, "formal, polished and respectful"},
    {"casual", {"casual", "friendly", "chill", "informal", "conversational"}, "warm, relaxed and conversational"},
    {"persuasive", {"persuasive", "convince", "sell", "pitch", "compelling", "marketing"}, "persuasive and confident, led by benefits"},
    {"humorous", {"funny", "humorous", "witty", "joke", "sarcastic"}, "witty and light, never forced"},
    {"empathetic", {"empathetic", "sensitive", "gentle", "compassionate", "apology", "sorry"}, "empathetic, gentle and sincere"},
    {"academic", {"academic", "scholarly", "research", "rigorous"}, "academic, precise and well-grounded"},
    {"simple", {"simple", "beginner", "easy", "eli5", "layman", "kid", "child", "basic"}, "plain-language and beginner-friendly, without jargon"},
    {"direct", {"direct", "blunt", "straight", "no fluff", "to the point", "brutal"}, "direct and blunt, with zero padding"},
};

static const vector<string> AUDIENCES = {"beginner", "kid", "student", "expert", "client", "boss", "manager",
                                         "recruiter", "customer", "investor", "teacher", "interviewer",
                                         "friend", "team", "professor"};

static const vector<string> HINGLISH = {"kaise", "kya", "mujhe", "hai", "karo", "banao", "batao", "samjhao",
                                        "kyun", "nahi", "yaar", "bhai", "aur", "chahiye", "wala", "mera",
                                        "tum", "aap", "kar do", "ke liye", "mere"};

static const vector<string> CONSTRAINT_MARKERS = {
    "must", "should", "don't", "do not", "never", "only", "without", "avoid", "at least", "at most",
    "no more than", "make sure", "ensure", "exactly", "not use", "no", "always", "need to", "have to"};

static const vector<string> GENERIC_PADS = {
    "Treat every detail of the request as deliberate, because the person chose each word to express something they want, and the best answer respects that intent fully.",
    "Aim for a result that can be used immediately, without the person having to rephrase, reformat or ask a second question to get what they originally wanted.",
    "Prefer specifics over generalities throughout, since concrete and relevant detail is what separates an excellent answer from a merely acceptable one.",
    "Keep the whole response coherent and internally consistent, so that nothing in the later part contradicts or quietly undoes what was established in the earlier part."};

// ----------------------------------------------------------------------------
// Cleaning
// ----------------------------------------------------------------------------
static const map<string, string> FIX = {
    {"pls", "please"}, {"plz", "please"}, {"plss", "please"}, {"u", "you"}, {"ur", "your"},
    {"im", "I'm"}, {"dont", "don't"}, {"cant", "can't"}, {"wont", "won't"}, {"doesnt", "doesn't"},
    {"isnt", "isn't"}, {"ive", "I've"}, {"thx", "thanks"}, {"wanna", "want to"}, {"gonna", "going to"},
    {"i", "I"}, {"bro", ""}, {"dude", ""}, {"yaar", ""}, {"bhai", ""},
};

static bool looksLikeCode(const string &line) {
    string t = trim(line);
    if (t.empty()) return false;
    if (line[0] == '\t' || line.rfind("    ", 0) == 0) return true;
    int sym = 0;
    for (char c : t)
        if (string("(){}[];=<>#*/&|").find(c) != string::npos) sym++;
    if (sym >= 2) return true;
    static const vector<string> starts = {"int ", "float ", "double ", "char ", "let ", "const ", "var ",
                                          "for(", "for (", "while(", "while (", "else", "print(", "printf",
                                          "cout", "cin", "std::", "using ", "return", "def ", "#include",
                                          "public ", "private ", "void ", "elif", "try:", "except", "import "};
    string lo = lower(t);
    for (auto &s : starts)
        if (lo.rfind(s, 0) == 0) return true;
    char last = t.back();
    return last == ';' || last == '{' || last == '}';
}

static string cleanText(const string &raw) {
    istringstream in(collapse(raw));
    string tok, out;
    while (in >> tok) {
        size_t a = 0, b = tok.size();
        while (a < b && ispunct((unsigned char)tok[a])) a++;
        while (b > a && ispunct((unsigned char)tok[b - 1])) b--;
        string lead = tok.substr(0, a), core = tok.substr(a, b - a), tail = tok.substr(b);
        auto it = FIX.find(lower(core));
        if (!core.empty() && it != FIX.end()) {
            if (it->second.empty()) continue;
            core = it->second;
        }
        if (!out.empty()) out += ' ';
        out += lead + core + tail;
    }
    bool cap = true;
    for (auto &c : out) {
        if (cap && isalpha((unsigned char)c)) {
            c = (char)toupper((unsigned char)c);
            cap = false;
        }
        if (c == '.' || c == '!' || c == '?') cap = true;
    }
    if (!out.empty() && !ispunct((unsigned char)out.back())) out += '.';
    return out;
}

static vector<string> splitFragments(const string &s) {
    vector<string> v;
    string cur;
    for (char c : s) {
        if (c == '.' || c == '!' || c == '?' || c == ';' || c == ',' || c == '\n') {
            if (trim(cur).size() > 3) v.push_back(trim(cur));
            cur.clear();
        } else {
            cur += c;
        }
    }
    if (trim(cur).size() > 3) v.push_back(trim(cur));
    return v;
}

static string explicitCount(const string &low) {
    static const set<string> units = {"words", "word", "sentences", "sentence", "lines", "line", "paragraphs",
                                      "paragraph", "points", "steps", "items", "ideas", "minutes", "pages",
                                      "slides", "characters", "bullets", "examples"};
    istringstream in(low);
    vector<string> w;
    string t;
    while (in >> t) {
        while (!t.empty() && ispunct((unsigned char)t.back())) t.pop_back();
        if (!t.empty()) w.push_back(t);
    }
    for (size_t i = 0; i + 1 < w.size(); ++i) {
        bool dig = all_of(w[i].begin(), w[i].end(), [](char c) { return isdigit((unsigned char)c); });
        if (dig && units.count(w[i + 1])) return w[i] + " " + w[i + 1];
    }
    return "";
}

static set<string> tokens(const string &s) {
    set<string> t;
    string cur;
    for (char c : lower(s)) {
        if (isalnum((unsigned char)c) || c == '+' || c == '#' || c == '_' || c == '\'') cur += c;
        else {
            if (cur.size() >= 3) t.insert(cur);
            cur.clear();
        }
    }
    if (cur.size() >= 3) t.insert(cur);
    return t;
}

// ----------------------------------------------------------------------------
// Analysis
// ----------------------------------------------------------------------------
struct Analysis {
    string rawRequest, request, material;
    Intent intent;
    string secondary, language, tone, audience, formatHint, langNote, count;
    vector<string> constraints;
    vector<string> extra;  // exact terms re-inserted if the fidelity check finds any loss
    bool brief = false, detail = false;
    int score = 0;
    string tier;
};

static Analysis analyse(const string &raw) {
    Analysis a;
    istringstream in(raw);
    string line, req;
    while (getline(in, line)) {
        if (looksLikeCode(line)) a.material += line + "\n";
        else req += line + "\n";
    }
    if (trim(req).empty()) {
        req = raw;
        a.material.clear();
    }
    a.rawRequest = req;
    a.request = cleanText(req);
    string low = lower(a.request);

    for (auto &l : LANGS)
        if (hits(low, l.second) > 0) { a.language = l.first; break; }

    auto list = intents();
    int best = -1, second = -1;
    size_t bi = 0, si = 0;
    for (size_t i = 0; i < list.size(); ++i) {
        int sc = hits(low, list[i].keys) * list[i].weight;
        if (!a.material.empty() && (list[i].name == "debugging" || list[i].name == "coding")) sc += 1;
        if (!a.language.empty() && list[i].name == "coding") sc += 2;
        if (sc > best) {
            second = best; si = bi;
            best = sc; bi = i;
        } else if (sc > second) {
            second = sc; si = i;
        }
    }
    if (best <= 0) a.intent = generalIntent();
    else {
        a.intent = list[bi];
        bool redundant = (list[bi].name == "debugging" && list[si].name == "coding") ||
                         (list[bi].name == "coding" && list[si].name == "debugging");
        if (second > 0 && si != bi && !redundant) a.secondary = list[si].name;
    }

    for (auto &t : TONES)
        if (hits(low, t.keys) > 0) { a.tone = t.text; break; }
    if (a.tone.empty()) a.tone = a.intent.defTone;
    for (auto &x : AUDIENCES)
        if (has(low, x)) { a.audience = x; break; }

    if (has(low, "table")) a.formatHint = "Present the core content as a table.";
    else if (has(low, "json")) a.formatHint = "Return valid JSON only, with no text outside it.";
    else if (has(low, "step by step") || has(low, "step-by-step") || has(low, "steps"))
        a.formatHint = "Use a numbered, step-by-step structure.";
    else if (has(low, "bullet") || has(low, "bullets") || has(low, "list"))
        a.formatHint = "Use a clean bulleted list for the main content.";
    else if (has(low, "one line") || has(low, "one sentence") || has(low, "one-liner"))
        a.formatHint = "Answer in a single sentence.";
    else if (has(low, "paragraph") || has(low, "essay"))
        a.formatHint = "Use flowing paragraphs rather than bullet points.";

    a.brief = hits(low, {"short", "brief", "quick", "concise", "tldr", "one line", "one sentence",
                         "in a nutshell", "briefly"}) > 0;
    a.detail = hits(low, {"detailed", "in depth", "in-depth", "comprehensive", "thorough", "long", "deep dive",
                          "extensive", "complete guide", "everything about"}) > 0;
    a.count = explicitCount(low);

    if (raw.find("\xE0\xA4") != string::npos || raw.find("\xE0\xA5") != string::npos)
        a.langNote = "The request is written in Hindi, so reply in Hindi (Devanagari script) while keeping code and technical terms in English.";
    else if (hits(low, HINGLISH) >= 2)
        a.langNote = "The request is written in Hinglish, so reply in natural Hinglish (Roman script) while keeping code, identifiers and technical terms in English.";

    auto frags = splitFragments(a.request);
    if (frags.size() > 1)
        for (auto &f : frags)
            if (hits(lower(f), CONSTRAINT_MARKERS) > 0) a.constraints.push_back(f);

    int words = wordCount(a.request);
    int score = words / 12 + (int)a.constraints.size();
    if (a.intent.name == "coding" || a.intent.name == "planning" || a.intent.name == "analysis") score += 2;
    if (a.intent.name == "debugging" || a.intent.name == "math/data") score += 1;
    if (!a.material.empty()) score += 1;
    if (a.detail) score += 2;
    if (count(a.request.begin(), a.request.end(), '?') > 1) score += 1;
    if (!a.secondary.empty()) score += 1;
    if (a.brief) score -= 3;
    a.score = score;
    a.tier = (a.brief || score <= 1) ? "compact" : (score <= 4 ? "standard" : "comprehensive");
    return a;
}

// ----------------------------------------------------------------------------
// Prompt builder: every prose block is a long, complete paragraph
// ----------------------------------------------------------------------------
struct Block {
    string text;
    bool prose;  // false = verbatim material (fenced code), exempt from the line-count rule
};

static string roleOf(const Analysis &a) {
    return replaceAll(a.intent.role, "{lang}", a.language.empty() ? "software" : a.language);
}

static string ordinalList(const vector<string> &items) {
    static const char *ord[] = {"first", "second", "third", "fourth", "fifth", "sixth"};
    string s;
    for (size_t i = 0; i < items.size(); ++i) {
        if (i) s += (i + 1 == items.size()) ? "; and " : "; ";
        s += string(i < 6 ? ord[i] : "next") + ", " + items[i];
    }
    return s;
}

static string sequence(const vector<string> &it) {
    string s = "begin by " + it[0];
    static const char *links[] = {"; then move on to ", "; after that, focus on ", "; next, concentrate on "};
    for (size_t i = 1; i + 1 < it.size(); ++i) s += string(links[(i - 1) % 3]) + it[i];
    if (it.size() > 1) s += "; and finish by " + it.back();
    return s;
}

static string ensureMin(string p) {
    size_t i = 0;
    while ((int)wrapLines(p, WRAP_WIDTH).size() < MIN_LINES && i < GENERIC_PADS.size()) p += " " + GENERIC_PADS[i++];
    return p;
}

static vector<Block> buildBlocks(const Analysis &a) {
    const Intent &I = a.intent;
    string role = roleOf(a);

    // P1: role, mission, the request itself, and the underlying desire
    string p1 = "You are " + article(role) + role +
                ", and for this task I need you to apply that full expertise rather than give a generic, surface-level answer. " +
                I.mission +
                ". Here is the request exactly as it was given, with only spelling and shorthand tidied up and nothing removed or reinterpreted: \"" +
                a.request + "\" What the person ultimately wants from this is " + I.desire +
                ", so aim for that outcome and judge your own answer against it before you present it.";
    if (!a.secondary.empty())
        p1 += " The request also touches on " + a.secondary + ", so cover that angle wherever it genuinely helps.";

    // Constraints and material
    string cons;
    if (!a.constraints.empty()) {
        vector<string> q;
        for (auto &c : a.constraints) q.push_back("\"" + c + "\"");
        cons = "These constraints come straight from the request and are non-negotiable, so honour each of them literally: " +
               ordinalList(q) +
               ". Beyond them, every other detail in the request, including any names, numbers, formats, limits and preferences, must be treated as a requirement, and none of it may be dropped, softened or reinterpreted.";
    } else {
        cons = "The request does not list separate prohibitions, which means its entire wording is the specification: every name, number, format, limit and preference it contains must be treated as a firm requirement, and nothing in it may be dropped, softened, shortened or reinterpreted in your answer. Where the wording is specific, follow it precisely, and where it leaves room, choose the option that best serves the outcome described above.";
    }
    if (!a.extra.empty())
        cons += " Pay particular attention to these exact terms from the original wording: " + joinWith(a.extra, ", ") + ".";
    if (!a.material.empty())
        cons += " The person has also supplied material, reproduced exactly below, and it must be treated as the primary source: read all of it before responding, keep every relevant detail in it intact, and do not invent anything that it does not contain.";

    // Audience, tone, language
    string aud = a.audience.empty()
                     ? "Write for an intelligent reader who wants the substance immediately, with no padding, no hand-holding and no unexplained jargon."
                     : "Write specifically for the " + a.audience +
                           ", adjusting vocabulary, depth and examples to what that reader already knows and what they need next.";
    aud += " The tone should be " + a.tone +
           ", and it should stay consistent from the first sentence to the last instead of drifting between registers.";
    if (!a.langNote.empty()) aud += " " + a.langNote;

    // Format and length
    string lenS;
    if (a.tier == "compact")
        lenS = "The final answer itself should stay compact: include everything needed to be complete and correct, and cut everything else.";
    else if (a.tier == "standard")
        lenS = "Make the answer thorough yet efficient, covering everything the request requires without ever repeating yourself.";
    else
        lenS = "Make the answer comprehensive and well organised: depth matters more than brevity here, but every sentence must still earn its place.";
    if (!a.count.empty()) lenS += " The requested size is " + a.count + ", which is a firm target rather than a suggestion.";
    string fmt = "Shape the final answer as follows. " + I.format + ".";
    if (!a.formatHint.empty()) fmt += " " + a.formatHint;
    fmt += " " + lenS;

    // Approach, quality, ambiguity
    string appr = "Work through this in a deliberate order rather than answering off the cuff: " + sequence(I.approach) +
                  ". Doing it in this order is what keeps the final result accurate, complete and free of avoidable mistakes.";
    string qual = "The standard I expect is demanding, and I will judge the result against it: " + ordinalList(I.quality) +
                  ". In addition, skip any preamble, filler or moralising and begin directly with the substance, because every line of the answer should be useful to the reader.";
    string unc;
    if (a.tier == "comprehensive")
        unc = "If a missing detail would change the answer in a meaningful way, you may ask at most two sharp clarifying questions before you begin; otherwise state your assumptions in a single line and proceed. Before delivering, reread the request one final time, check every stated detail against your answer, fix any gap you find, and then present only the finished result.";
    else
        unc = "If any detail is genuinely missing, state the assumption you are making in a single line and carry on instead of stalling with questions, because the person wants progress rather than a back-and-forth. Before you finish, check your answer once more against every detail of the request.";

    vector<Block> b;
    auto P = [&](const string &s) { b.push_back({ensureMin(s), true}); };
    auto M = [&]() {
        if (!a.material.empty()) b.push_back({"```\n" + a.material + "```", false});
    };

    if (a.tier == "compact") {
        P(p1);
        P(cons + " " + aud + " " + fmt);
        M();
        P(qual + " " + unc);
    } else if (a.tier == "standard") {
        P(p1);
        P(cons);
        M();
        P(aud);
        P(fmt);
        P(qual + " " + unc);
    } else {
        P(p1);
        P(cons);
        M();
        P(aud);
        P(fmt);
        P(appr);
        P(qual);
        P(unc);
    }
    return b;
}

static string render(const vector<Block> &b, bool wrapped) {
    string out;
    for (size_t i = 0; i < b.size(); ++i) {
        if (i) out += "\n\n";
        if (b[i].prose && wrapped) out += joinWith(wrapLines(b[i].text, WRAP_WIDTH), "\n");
        else out += b[i].text;
    }
    return out;
}

// ----------------------------------------------------------------------------
// Verification
// ----------------------------------------------------------------------------
static vector<string> missingDetails(const Analysis &a, const vector<Block> &b) {
    set<string> outTok = tokens(render(b, false));
    vector<string> miss;
    for (auto &t : tokens(a.rawRequest)) {
        if (FIX.count(t)) continue;  // shorthand intentionally normalised or filler intentionally dropped
        if (!outTok.count(t)) miss.push_back(t);
    }
    return miss;
}

static int shortestParagraph(const vector<Block> &b) {
    int m = 1 << 30;
    for (auto &x : b)
        if (x.prose) m = min(m, (int)wrapLines(x.text, WRAP_WIDTH).size());
    return m == (1 << 30) ? 0 : m;
}

struct Result {
    Analysis a;
    vector<Block> blocks;
    int paragraphs = 0, shortest = 0;
    bool fidelity = true;
};

static Result process(const string &raw) {
    Result r;
    r.a = analyse(raw);
    r.blocks = buildBlocks(r.a);
    auto miss = missingDetails(r.a, r.blocks);
    if (!miss.empty()) {  // self-repair: re-insert the lost terms and rebuild
        r.a.extra = miss;
        r.blocks = buildBlocks(r.a);
        r.fidelity = missingDetails(r.a, r.blocks).empty();
    }
    for (auto &x : r.blocks)
        if (x.prose) r.paragraphs++;
    r.shortest = shortestParagraph(r.blocks);
    return r;
}

// ----------------------------------------------------------------------------
// I/O
// ----------------------------------------------------------------------------
static bool readPrompt(string &out) {
    out.clear();
    string line;
    int lines = 0, blanks = 0;
    while (getline(cin, line)) {
        string t = trim(line);
        if (t == "END") break;
        if (t.empty()) {
            if (lines == 0) continue;
            if (lines == 1) break;
            if (++blanks >= 2) break;
            out += '\n';
            continue;
        }
        blanks = 0;
        lines++;
        out += line + '\n';
    }
    return !trim(out).empty();
}

static int selfTest() {
    const vector<string> tests = {
        "explain recursion",
        "bro pls make a c++ program for bank management, it must use classes and file handling, dont use global variables",
        "write a short email to my boss asking for leave next friday, keep it polite",
        "mujhe python mein ek web scraper banao jo har ghante data nikale, aur csv mein save kare",
        "fix this bug pls\nint main(){\n int a=5\n return a;\n}\n",
        "compare react vs vue for a small startup, i need a detailed answer with pros and cons in a table, and give a clear recommendation, dont be vague",
        "plan my 3 month study schedule for gate exam, i have only 4 hours daily and weak in os and networks",
        "ideas for a gaming channel name",
        "summarize the key points of the french revolution in 5 points",
        "solve 3x + 7 = 25 and show steps",
        "what should i eat today",
        "write a 500 words story about a lighthouse keeper who finds a message in a bottle, tone should be emotional and the ending must be hopeful, avoid cliches, no more than 3 characters, make sure the setting is a real coastline in scotland",
    };
    int fails = 0, n = 1;
    for (auto &t : tests) {
        Result r = process(t);
        bool ok = r.shortest >= MIN_LINES && r.fidelity && r.paragraphs >= 3;
        if (!ok) fails++;
        cout << (ok ? "PASS" : "FAIL") << "  #" << n++ << "  intent=" << r.a.intent.name << "  tier=" << r.a.tier
             << "  paragraphs=" << r.paragraphs << "  shortest=" << r.shortest << " lines"
             << "  fidelity=" << (r.fidelity ? "100%" : "LOSS") << "\n";
    }
    cout << "\n" << (tests.size() - fails) << "/" << tests.size() << " checks passed\n";
    return fails ? 1 : 0;
}

int main(int argc, char **argv) {
    if (argc > 1 && string(argv[1]) == "--selftest") return selfTest();

    cout << "==============================================\n"
         << "          PROMPT POLISHER v2 (C++)\n"
         << "==============================================\n"
         << "Paste your raw prompt, then type END on its own line\n"
         << "(one-liners: just press Enter on an empty line).\n"
         << "Type 'exit' to quit.\n";

    while (true) {
        cout << "\nRAW PROMPT > " << flush;
        string raw;
        if (!readPrompt(raw)) break;
        string t = lower(trim(raw));
        if (t == "exit" || t == "quit") break;

        Result r = process(raw);
        cout << "\n[detected] intent=" << r.a.intent.name;
        if (!r.a.secondary.empty()) cout << " (+" << r.a.secondary << ")";
        if (!r.a.language.empty()) cout << ", language=" << r.a.language;
        cout << ", length=" << r.a.tier << "\n";
        cout << "[checks]   paragraphs=" << r.paragraphs << " | shortest=" << r.shortest << " lines (minimum "
             << MIN_LINES << ") | detail fidelity=" << (r.fidelity ? "100%" : "LOSS DETECTED") << "\n";
        cout << "\n---------------- COPY FROM HERE ----------------\n"
             << render(r.blocks, true) << "\n"
             << "---------------- COPY UNTIL HERE ---------------\n";

        ofstream f("polished_prompt.txt");
        if (f) {
            f << render(r.blocks, false) << "\n";
            cout << "\nSaved to polished_prompt.txt (one line per paragraph, ideal for pasting)\n";
        }
    }
    cout << "\nBye.\n";
    return 0;
}
