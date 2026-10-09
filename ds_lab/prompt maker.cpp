// prompt_polisher.cpp
// Turns a raw, messy prompt into a polished, context-specific prompt that can be
// pasted into any AI chatbot. No external libraries, no API calls.
//
// Build:  g++ -std=c++17 -O2 prompt_polisher.cpp -o prompt_polisher
// Run:    ./prompt_polisher
//
// Input : type/paste your raw prompt, then finish with a line containing only END
//         (for a one-liner, just press Enter on an empty line).  Type "exit" to quit.
// Output: printed on screen and saved to polished_prompt.txt

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

// ----------------------------------------------------------------------------
// Small string helpers
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

// Whole-word / whole-phrase match (works for "c++", "pros and cons", etc.)
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

// ----------------------------------------------------------------------------
// Intent catalogue
// ----------------------------------------------------------------------------
struct Intent {
    string name;
    vector<string> keys;
    int weight;
    string role;     // "{lang}" is replaced by the detected programming language
    string defTone;
    string format;
    vector<string> quality;
    vector<string> approach;
};

static vector<Intent> intents() {
    return {
        {"coding",
         {"code", "program", "script", "function", "class", "algorithm", "implement", "build", "app",
          "website", "api", "library", "module", "software", "backend", "frontend", "database", "game",
          "bot", "automation", "cli", "project", "compile", "software", "system", "application"},
         1,
         "senior {lang} engineer",
         "precise and technical, no fluff",
         "Complete, runnable code in fenced code blocks, then a short explanation of how it works and how to run it.",
         {"The code must run as written: no placeholders, no pseudo-code, no omitted parts",
          "Clear naming and sensible structure; comments only where they add value",
          "Validate input and handle edge cases and failures",
          "Follow the idioms and best practices of the language"},
         {"Restate the requirements in one line",
          "Choose the simplest design that fully satisfies them",
          "Write the complete code",
          "Mentally test it on normal and edge-case inputs before answering"}},

        {"debugging",
         {"bug", "error", "fix", "debug", "crash", "exception", "traceback", "not working", "broken",
          "fails", "failing", "issue", "segfault", "wrong output", "doesn't work", "doesnt work"},
         2,
         "expert {lang} debugger and code reviewer",
         "direct and diagnostic",
         "Root cause first, then the corrected code, then one short note on how to prevent it.",
         {"Identify the actual root cause, not just the symptom",
          "Show the minimal fix and the full corrected code where needed",
          "Point out any other bugs or risks noticed along the way"},
         {"Read the code and the error carefully",
          "Locate the exact line or logic responsible",
          "Explain why it fails",
          "Provide and sanity-check the fix"}},

        {"writing",
         {"write", "essay", "email", "letter", "article", "blog", "post", "story", "poem", "caption",
          "speech", "cover letter", "resume", "report", "message", "draft", "bio", "description",
          "linkedin", "newsletter", "copy", "slogan", "application"},
         1,
         "professional writer and editor",
         "natural, human and polished",
         "The finished piece only, ready to use, with no commentary around it.",
         {"Sound like a real person, not a template: concrete, specific, varied sentences",
          "No clichés, filler openers or generic buzzwords",
          "Consistent voice from the first line to the last",
          "Ready to send or publish without further editing"},
         {"Identify the purpose and the reader",
          "Pick the strongest angle and open with it",
          "Write the piece",
          "Cut anything that does not earn its place"}},

        {"explanation",
         {"explain", "what is", "what are", "how does", "how do", "why", "teach", "understand", "learn",
          "eli5", "concept", "meaning", "difference between", "tell me about"},
         1,
         "patient expert teacher",
         "clear, concrete and encouraging",
         "Start with a one-sentence answer, then build the explanation in layers with concrete examples.",
         {"Accurate first, simple second: never trade correctness for brevity",
          "At least one concrete example or analogy",
          "Define any technical term the first time it appears",
          "End with the most common misconception or pitfall"},
         {"Give the core idea in one sentence",
          "Expand step by step from basics to nuance",
          "Illustrate with an example",
          "Check that nothing relies on unexplained jargon"}},

        {"analysis",
         {"analyze", "analyse", "evaluate", "review", "critique", "assess", "compare", "pros and cons",
          "versus", "vs", "better", "which is", "audit", "feedback", "worth it"},
         1,
         "sharp, impartial analyst",
         "objective and evidence-driven",
         "A verdict up front, then the reasoning, then the trade-offs. Use a table when comparing options.",
         {"Commit to a clear recommendation and justify it",
          "Base every claim on evidence or explicit reasoning; flag what cannot be known",
          "Cover the strongest counter-argument",
          "No vague both-sides filler"},
         {"Define the criteria that matter for this decision",
          "Evaluate each option against them",
          "Weigh the trade-offs",
          "Conclude with a decisive recommendation"}},

        {"planning",
         {"plan", "roadmap", "schedule", "strategy", "itinerary", "routine", "workflow", "timeline",
          "checklist", "steps to", "how to start", "organize", "prepare", "goal", "study plan"},
         1,
         "experienced strategist and planner",
         "practical and decisive",
         "A prioritised, time-ordered plan with concrete actions, durations and checkpoints.",
         {"Every step must be an action someone can start today",
          "Realistic timings and dependencies",
          "Prioritise ruthlessly: what matters most comes first",
          "Include how to measure progress"},
         {"Clarify the end goal and constraints",
          "Break it into phases",
          "Order actions by priority and dependency",
          "Add checkpoints and a fallback"}},

        {"summarizing",
         {"summarize", "summarise", "summary", "tldr", "shorten", "condense", "key points", "brief me",
          "recap"},
         2,
         "precise summarizer",
         "neutral and compact",
         "The summary only, in the requested shape, starting with the single most important point.",
         {"Keep every key fact and decision; drop repetition and examples",
          "Do not add information that is not in the source",
          "Preserve the original meaning and tone"},
         {"Identify the main point and supporting points",
          "Compress without distortion",
          "Verify nothing important was lost"}},

        {"brainstorming",
         {"ideas", "brainstorm", "suggest", "names", "options", "recommend", "alternatives", "creative",
          "concepts", "inspiration"},
         1,
         "inventive creative strategist",
         "bold, fresh and specific",
         "A ranked list; each idea gets a one-line pitch and the reason it works.",
         {"Ideas must be distinct from each other, not variations of one theme",
          "Specific and original, avoiding the obvious first-page answers",
          "Rank them and mark the single best pick"},
         {"Generate a wide spread of directions",
          "Discard the generic ones",
          "Rank the survivors and explain the top pick"}},

        {"rewriting",
         {"translate", "rewrite", "rephrase", "proofread", "improve", "polish", "paraphrase", "grammar",
          "edit", "correct"},
         2,
         "expert editor and translator",
         "faithful to the original voice, but sharper",
         "Only the final text, followed by at most three short bullets listing the main changes.",
         {"Preserve the original meaning and intent exactly",
          "Fix grammar, flow and word choice",
          "Keep the author's voice instead of replacing it with a generic one"},
         {"Understand what the text is trying to achieve",
          "Rework it for clarity and impact",
          "Compare against the original to confirm nothing changed in meaning"}},

        {"math/data",
         {"calculate", "solve", "equation", "math", "statistics", "data", "formula", "excel",
          "spreadsheet", "probability", "derive", "proof", "integral"},
         1,
         "rigorous mathematician and data analyst",
         "exact and methodical",
         "Final answer clearly stated, preceded by the working shown step by step.",
         {"Show every step so the result can be checked",
          "State assumptions and units explicitly",
          "Verify the result with a second method or a sanity check"},
         {"Restate the problem formally",
          "Solve step by step",
          "Verify the answer independently"}},
    };
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
    {"Python", {"python", "py", "django", "flask", "pandas", "numpy"}},
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
    {"formal", {"formal", "professional", "official", "corporate", "polite"},
     "formal, polished and respectful"},
    {"casual", {"casual", "friendly", "chill", "informal", "conversational"},
     "warm, relaxed and conversational"},
    {"persuasive", {"persuasive", "convince", "sell", "pitch", "compelling", "marketing"},
     "persuasive and confident, benefit-led"},
    {"humorous", {"funny", "humorous", "witty", "joke", "sarcastic"}, "witty and light, never forced"},
    {"empathetic", {"empathetic", "sensitive", "gentle", "compassionate", "apology", "sorry"},
     "empathetic, gentle and sincere"},
    {"academic", {"academic", "scholarly", "research", "rigorous"},
     "academic, precise and well-referenced in style"},
    {"simple", {"simple", "beginner", "easy", "eli5", "layman", "kid", "child", "basic"},
     "plain-language, beginner-friendly, no jargon"},
    {"direct", {"direct", "blunt", "straight", "no fluff", "to the point", "brutal"},
     "direct and blunt, zero padding"},
};

static const vector<string> AUDIENCES = {"beginner", "kid",       "student",   "expert",   "client",
                                         "boss",     "manager",   "recruiter", "customer", "investor",
                                         "teacher",  "interviewer", "friend",  "team",     "professor"};

static const vector<string> HINGLISH = {"kaise", "kya", "mujhe", "hai", "karo", "banao", "batao",
                                        "samjhao", "kyun", "nahi", "yaar", "bhai", "aur", "chahiye",
                                        "wala", "mera", "tum", "aap", "kar do", "ke liye", "mere"};

static const vector<string> CONSTRAINT_MARKERS = {
    "must", "should", "don't", "do not", "never", "only", "without", "avoid", "at least", "at most",
    "no more than", "make sure", "ensure", "exactly", "not use", "no", "always", "need to", "have to"};

// ----------------------------------------------------------------------------
// Cleaning
// ----------------------------------------------------------------------------
static bool looksLikeCode(const string &line) {
    string t = trim(line);
    if (t.empty()) return false;
    if (line.size() > 1 && (line[0] == '\t' || line.rfind("    ", 0) == 0)) return true;
    static const vector<string> sig = {"#include", "int main", "def ", "class ", "public ", "function ",
                                       "=>", "console.log", "printf", "cout <<", "import ", "return ",
                                       "SELECT ", "{", "}"};
    for (auto &s : sig)
        if (t.find(s) != string::npos) return true;
    char last = t.back();
    return last == ';' || last == '{' || last == '}';
}

static string cleanText(const string &raw) {
    static const map<string, string> fix = {
        {"pls", "please"}, {"plz", "please"},   {"u", "you"},         {"ur", "your"},
        {"im", "I'm"},     {"dont", "don't"},   {"cant", "can't"},    {"wont", "won't"},
        {"doesnt", "doesn't"}, {"isnt", "isn't"}, {"ive", "I've"},    {"thx", "thanks"},
        {"wanna", "want to"}, {"gonna", "going to"}, {"i", "I"},      {"bro", ""},
        {"dude", ""},      {"yaar", ""},        {"bhai", ""},         {"plss", "please"},
    };
    istringstream in(collapse(raw));
    string tok, out;
    while (in >> tok) {
        size_t a = 0, b = tok.size();
        while (a < b && ispunct((unsigned char)tok[a])) a++;
        while (b > a && ispunct((unsigned char)tok[b - 1])) b--;
        string lead = tok.substr(0, a), core = tok.substr(a, b - a), tail = tok.substr(b);
        auto it = fix.find(lower(core));
        if (!core.empty() && it != fix.end()) {
            if (it->second.empty()) continue;
            core = it->second;
        }
        if (!out.empty()) out += ' ';
        out += lead + core + tail;
    }
    // capitalise sentence starts
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
    static const set<string> units = {"words", "word",   "sentences", "sentence", "lines",   "line",
                                      "paragraphs", "paragraph", "points", "steps", "items", "ideas",
                                      "minutes", "pages",  "slides",    "characters", "bullets", "examples"};
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

// ----------------------------------------------------------------------------
// Analysis
// ----------------------------------------------------------------------------
struct Analysis {
    string request, material;
    Intent intent;
    string secondary, language, tone, audience, formatHint, langNote, count;
    vector<string> constraints;
    bool brief = false, detail = false;
    int score = 0;
    string tier;
};

static Analysis analyse(const string &raw) {
    Analysis a;

    // separate pasted code/material from the actual request
    istringstream in(raw);
    string line, req;
    while (getline(in, line)) {
        if (looksLikeCode(line)) a.material += line + "\n";
        else req += line + "\n";
    }
    if (trim(req).empty()) req = raw, a.material.clear();  // everything looked like code: treat as request
    a.request = cleanText(req);
    string low = lower(a.request);

    // intent scoring
    auto list = intents();
    int best = -1, second = -1;
    size_t bi = 0, si = 0;
    for (size_t i = 0; i < list.size(); ++i) {
        int sc = hits(low, list[i].keys) * list[i].weight;
        if (!a.material.empty() && (list[i].name == "debugging" || list[i].name == "coding")) sc += 1;
        if (sc > best) {
            second = best; si = bi;
            best = sc; bi = i;
        } else if (sc > second) {
            second = sc; si = i;
        }
    }
    if (best <= 0) {
        a.intent = {"general",
                    {},
                    1,
                    "knowledgeable, resourceful expert in whatever domain this request belongs to",
                    "clear, direct and helpful",
                    "The answer first, then only the supporting detail that is needed.",
                    {"Accurate and specific to this exact request, not generic",
                     "Say clearly what cannot be known instead of guessing",
                     "Nothing padded: every sentence must earn its place"},
                    {"Work out what the person actually needs",
                     "Answer that directly",
                     "Add only the context that changes what they will do next"}};
    } else {
        a.intent = list[bi];
        if (second > 0 && si != bi) a.secondary = list[si].name;
    }

    // programming language
    for (auto &l : LANGS)
        if (hits(low, l.second) > 0) { a.language = l.first; break; }

    // tone
    for (auto &t : TONES)
        if (hits(low, t.keys) > 0) { a.tone = t.text; break; }
    if (a.tone.empty()) a.tone = a.intent.defTone;

    // audience
    for (auto &x : AUDIENCES)
        if (has(low, x)) { a.audience = x; break; }

    // format hints
    if (has(low, "table")) a.formatHint = "Present the core content as a table.";
    else if (has(low, "json")) a.formatHint = "Return valid JSON only, with no text outside it.";
    else if (has(low, "step by step") || has(low, "step-by-step") || has(low, "steps"))
        a.formatHint = "Use numbered, step-by-step structure.";
    else if (has(low, "bullet") || has(low, "bullets") || has(low, "list"))
        a.formatHint = "Use a clean bulleted list.";
    else if (has(low, "one line") || has(low, "one sentence") || has(low, "one-liner"))
        a.formatHint = "Answer in a single sentence.";
    else if (has(low, "paragraph") || has(low, "essay"))
        a.formatHint = "Use flowing paragraphs, not bullet points.";

    // length hints
    a.brief = hits(low, {"short", "brief", "quick", "concise", "tldr", "one line", "one sentence",
                         "in a nutshell", "briefly"}) > 0;
    a.detail = hits(low, {"detailed", "in depth", "in-depth", "comprehensive", "thorough", "long",
                          "deep dive", "extensive", "complete guide", "everything about"}) > 0;
    a.count = explicitCount(low);

    // language of the reply
    if (raw.find("\xE0\xA4") != string::npos || raw.find("\xE0\xA5") != string::npos)
        a.langNote = "The request is written in Hindi; reply in Hindi (Devanagari), keeping code and technical terms in English.";
    else if (hits(low, HINGLISH) >= 2)
        a.langNote = "The request is in Hinglish; reply in natural Hinglish (Roman script), keeping code, identifiers and technical terms in English.";

    // hard constraints
    auto frags = splitFragments(a.request);
    if (frags.size() > 1) {
        for (auto &f : frags)
            if (hits(lower(f), CONSTRAINT_MARKERS) > 0) a.constraints.push_back(f);
    }

    // complexity -> length tier
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
// Prompt builder
// ----------------------------------------------------------------------------
static string roleOf(const Analysis &a) {
    string r = a.intent.role;
    string lang = a.language.empty() ? "software" : a.language;
    return replaceAll(r, "{lang}", lang);
}

static string lengthLine(const Analysis &a) {
    string base;
    if (a.tier == "compact") base = "Keep it tight: only what is needed, nothing more.";
    else if (a.tier == "standard") base = "Be thorough but efficient.";
    else base = "Be comprehensive and well structured; depth matters more than brevity, but every sentence must earn its place.";
    if (!a.count.empty()) base = "Length: " + a.count + " (treat this as a firm target). " + base;
    return base;
}

static string article(const string &w) {
    return (!w.empty() && string("aeiouAEIOU").find(w[0]) != string::npos) ? "an " : "a ";
}

static string buildPrompt(const Analysis &a) {
    ostringstream o;
    string role = roleOf(a);

    if (a.tier == "compact") {
        o << "Act as " << article(role) << role << ". " << a.request << "\n\n";
        if (!a.material.empty()) o << "Material:\n```\n" << a.material << "```\n\n";
        if (!a.constraints.empty()) {
            o << "Hard constraints: ";
            for (size_t i = 0; i < a.constraints.size(); ++i) o << (i ? "; " : "") << a.constraints[i];
            o << "\n";
        }
        o << "Tone: " << a.tone << ".";
        if (!a.audience.empty()) o << " Audience: " << a.audience << ".";
        o << " ";
        o << (a.formatHint.empty() ? a.intent.format : a.formatHint) << " " << lengthLine(a) << " ";
        if (!a.langNote.empty()) o << a.langNote << " ";
        o << "Skip preamble and filler. If a detail is missing, state your assumption in one line and proceed.";
        return o.str();
    }

    o << "ROLE\nYou are " << article(role) << role << ". Your answer should be what a top specialist would give: accurate, specific and immediately usable.\n\n";
    o << "TASK\n" << a.request << "\n";
    if (!a.secondary.empty()) o << "(Secondary angle to cover where relevant: " << a.secondary << ".)\n";
    o << "\n";
    if (!a.material.empty()) o << "MATERIAL PROVIDED\n```\n" << a.material << "```\n\n";

    if (!a.constraints.empty()) {
        o << "HARD CONSTRAINTS (non-negotiable)\n";
        for (auto &c : a.constraints) o << "- " << c << "\n";
        o << "\n";
    }

    o << "AUDIENCE AND TONE\n";
    if (!a.audience.empty()) o << "Written for: " << a.audience << ". ";
    o << "Tone: " << a.tone << ".";
    if (!a.langNote.empty()) o << " " << a.langNote;
    o << "\n\n";

    o << "OUTPUT FORMAT\n" << a.intent.format;
    if (!a.formatHint.empty()) o << " " << a.formatHint;
    o << " " << lengthLine(a) << "\n\n";

    if (a.tier == "comprehensive") {
        o << "APPROACH\n";
        int n = 1;
        for (auto &s : a.intent.approach) o << n++ << ". " << s << "\n";
        o << "\n";
    }

    o << "QUALITY BAR\n";
    for (auto &q : a.intent.quality) o << "- " << q << "\n";
    o << "- Skip preamble, filler and moralising; start with the substance\n\n";

    if (a.tier == "comprehensive")
        o << "IF SOMETHING IS UNCLEAR\nIf a missing detail would materially change the answer, ask at most two sharp clarifying questions first. Otherwise state your assumptions in one line and proceed.\n";
    else
        o << "IF SOMETHING IS UNCLEAR\nState your assumption in one line and proceed; do not stall with questions.\n";
    return o.str();
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
            if (lines == 1) break;       // one-liner finished with Enter
            if (++blanks >= 2) break;    // two empty lines also end a long paste
            out += '\n';
            continue;
        }
        blanks = 0;
        lines++;
        out += line + '\n';
    }
    return !trim(out).empty();
}

int main() {
    cout << "==============================================\n"
         << "            PROMPT POLISHER (C++)\n"
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

        Analysis a = analyse(raw);
        string prompt = buildPrompt(a);

        cout << "\n[detected] intent=" << a.intent.name;
        if (!a.secondary.empty()) cout << " (+" << a.secondary << ")";
        if (!a.language.empty()) cout << ", language=" << a.language;
        cout << ", length=" << a.tier << " (complexity " << a.score << ")\n";
        cout << "\n---------------- COPY FROM HERE ----------------\n"
             << prompt << "\n"
             << "---------------- COPY UNTIL HERE ---------------\n";

        ofstream f("polished_prompt.txt");
        if (f) {
            f << prompt << "\n";
            cout << "\nSaved to polished_prompt.txt\n";
        }
    }
    cout << "\nBye.\n";
    return 0;
}