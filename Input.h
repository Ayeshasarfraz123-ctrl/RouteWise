#ifndef INPUT_H
#define INPUT_H

#include <iostream>
#include <string>
using namespace std;

// Becomes true if the input stream ends (so loops can stop safely)
inline bool& inputEnded() {
    static bool ended = false;
    return ended;
}

// Remove spaces, tabs and line-end characters from both ends
inline string trim(const string& s) {
    int start = 0;
    int end = (int)s.length();
    while (start < end && (s[start] == ' ' || s[start] == '\t' || s[start] == '\r' || s[start] == '\n'))
        start++;
    while (end > start && (s[end - 1] == ' ' || s[end - 1] == '\t' || s[end - 1] == '\r' || s[end - 1] == '\n'))
        end--;
    return s.substr(start, end - start);
}

// Show a question and read one whole line
inline string readLine(const string& prompt) {
    cout << prompt;
    string line;
    if (!getline(cin, line)) {
        inputEnded() = true;
        return "";
    }
    return trim(line);
}

// Turn text into a whole number. Returns false if it is not a valid number.
inline bool parseInt(const string& text, int& out) {
    string s = trim(text);
    int i = 0;
    bool negative = false;
    if (s.length() > 0 && s[0] == '-') {
        negative = true;
        i = 1;
    }
    if (i >= (int)s.length()) return false;
    if ((int)s.length() - i > 9) return false;   // too long
    int value = 0;
    for (; i < (int)s.length(); i++) {
        if (s[i] < '0' || s[i] > '9') return false;
        value = value * 10 + (s[i] - '0');
    }
    out = negative ? -value : value;
    return true;
}

// Ask until the user types a whole number between minValue and maxValue
inline int readInt(const string& prompt, int minValue, int maxValue) {
    while (!inputEnded()) {
        string line = readLine(prompt);
        if (inputEnded()) break;
        int value;
        if (parseInt(line, value) && value >= minValue && value <= maxValue)
            return value;
        cout << "  Please enter a whole number from " << minValue
            << " to " << maxValue << "." << endl;
    }
    return minValue;
}
// Turn text like "2.5" into a decimal number. Returns false if it is not valid.
inline bool parseDouble(const string& text, double& out) {
    string s = trim(text);
    if (s.length() == 0 || s.length() > 12) return false;
    bool afterDot = false;
    int digits = 0;
    double whole = 0;
    double fraction = 0;
    double scale = 1;
    for (int i = 0; i < (int)s.length(); i++) {
        char c = s[i];
        if (c == '.') {
            if (afterDot) return false;     // two dots
            afterDot = true;
        }
        else if (c >= '0' && c <= '9') {
            digits++;
            if (!afterDot) {
                whole = whole * 10 + (c - '0');
            }
            else {
                scale = scale / 10;
                fraction = fraction + (c - '0') * scale;
            }
        }
        else {
            return false;
        }
    }
    if (digits == 0) return false;
    out = whole + fraction;
    return true;
}

// Ask until the user types a decimal number between minValue and maxValue
inline double readDouble(const string& prompt, double minValue, double maxValue) {
    while (!inputEnded()) {
        string line = readLine(prompt);
        if (inputEnded()) break;
        double value;
        if (parseDouble(line, value) && value >= minValue && value <= maxValue)
            return value;
        cout << "  Please enter a number from " << minValue
            << " to " << maxValue << "." << endl;
    }
    return minValue;
}
#endif