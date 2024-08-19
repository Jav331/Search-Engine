/*This a .cpp file that takes in websit contents
 * and priortizes a simple serch engine for a collection
 * of pages wthin the website
 */
#include <iostream>
#include <fstream>
#include "error.h"
#include "filelib.h"
#include "map.h"
#include "search.h"
#include "set.h"
#include "simpio.h"
#include "strlib.h"
#include "vector.h"
#include "SimpleTest.h"
using namespace std;


/*
 * The 'cleanToken' function takes in a string token
 * and removes non-alpha or non-digit characters from
 * the token making it a "cleaned" version.
 */
string cleanToken(string s) {
    s = toLowerCase(s);

    string result;
    for (int i = 0; i < s.length(); i++) {
        if (isalpha(s[i]) || isdigit(s[i])) {
            result += s[i];
        }
    }
    return result;
}

/*
 * The function 'gatherTokens' takes a text string and
 * creates a Vector containing each individual words contained
 * within the text making sure not to duplicate any of the words.
 * The individual words are cleaned using the 'cleanToken' helper
 * function.
 */
Set<string> gatherTokens(string text) {
    Set<string> tokens;

    Vector<string> tokenizeString = stringSplit(text, " ");

    for (string token: tokenizeString) {
        string cleaned = cleanToken(token);
        // case when there is an empty string
        if (cleaned != "") {
            tokens.add(cleaned);
        }
    }
    return tokens;
}

/*
 * This function takes in a file and creates an inverted
 * index which maps a unique word to a set of URLs. The
 * function priotizes the organization of the file of
 * the contents of the url being place below the URL.
 * By iteratinb through the file and splitting the contents
 * using the gatherTokens function, an index can be created
 * which keeps track of the number of pages a word appears
 */
int buildIndex(string dbfile, Map<string, Set<string>>& index) {
    // structure will be word --> location

    /* The following code reads data from the file into a Vector
     * of strings representing the lines of the file.
     */
    ifstream in;

    if (!openFile(in, dbfile))
        error("Cannot open file named " + dbfile);

    Vector<string> lines = readLines(in);

    int numberPages = 0;

    for (int i = 0; i < lines.size() - 1; i+= 2) {

        string page = lines[i];
        string body = lines[i + 1];
        Set<string> uniqueTokens = gatherTokens(body);
        for (string token: uniqueTokens) {
            index[token].add(page);
        }
        numberPages++;
    }

    return numberPages;
}

/*
 * This function takes an inverted index and query string
 * which is a string containing for words being searched
 * for in a URL page. The return of this function are URLs
 * that pertain to that query. The query can handle for
 * words being searched that have URL pages in common (+),
 * no correlations (-), or words that are in union with each
 * other.
 */
Set<string> findQueryMatches(Map<string, Set<string>>& index, string query) {
    Set<string> result;
    Vector<string> queryInputs = stringSplit(query, " ");

    for (int i = 0; i < queryInputs.size(); i++) {

        string querySingleInput = queryInputs[i];
        string queryInputsCleaned = cleanToken(querySingleInput);
        // checks if there is only one word
        if (queryInputs.size() == 1) {
            result = index[queryInputsCleaned];
        }
        //otherwise
        else {
            if (queryInputs[i][0] == '+') {
                result.intersect(index[queryInputsCleaned]);
            }

            else if (queryInputs[i][0] == '-') {
                result.difference(index[queryInputsCleaned]);
            }
            else {
                result.unionWith(index[queryInputsCleaned]);
            }

        }

    }

    return result;
}

/*
 * The 'searchEngine' function priotizes the use of
 * the console and allows a user to input key words
 * that then searches through the data base file to find
 * pages that contain such words.
 */
void searchEngine(string dbfile) {

    ifstream in;
    Vector<string> allWords;

    if (openFile(in, dbfile)) {
        allWords = readLines(in);
    } else {
        error("Cannot open file: " + dbfile);
    }

    cout << "Stand by while building index..." << endl ;

    Map<string, Set<string>> index;
    int pageNumbers = buildIndex(dbfile, index);
    int uniqueQueryNumbers = index.size();

    cout << "Indexed " << pageNumbers << " pages containing " << uniqueQueryNumbers << " unique terms." << endl << endl;


    while (true){
        string inputQuery = getLine("Enter query sentence (RETURN/ENTER to quit): ");
        // User presses enter to stop
        if (inputQuery == "") {
            break;
        }
        Set<string> numberMatchingPages = findQueryMatches(index, inputQuery);
        cout << "Found " << numberMatchingPages.size() << " matching pages"<< endl;
        cout << "{";
        // Print out pages in new line format
        for (string page: numberMatchingPages) {
            cout << page << endl;
        }
        cout << "}" << endl;

        cout << endl;
    }
}

/* * * * * * Test Cases * * * * * */

// TODO: add your STUDENT_TEST test cases here!


/* Please not add/modify/remove the PROVIDED_TEST entries below.
 * Place your student tests cases above the provided tests.
 */

STUDENT_TEST("findQueryMatches from tiny.txt, random words") {
    Map<string, Set<string>> index;
    buildIndex("res/tiny.txt", index);
    Set<string> matchesRedOrFish = findQueryMatches(index, "Hello");
    EXPECT_EQUAL(matchesRedOrFish.size(), 0);
    Set<string> matchesRedAndFish = findQueryMatches(index, "Hello-Word");
    EXPECT_EQUAL(matchesRedAndFish.size(), 0);
    Set<string> matchesRedWithoutFish = findQueryMatches(index, "fishh");
    EXPECT_EQUAL(matchesRedWithoutFish.size(), 0);
}

STUDENT_TEST("findQueryMatches from tiny.txt, '-' ") {
    Map<string, Set<string>> index;
    buildIndex("res/tiny.txt", index);
    Set<string> matchesRedOrFish = findQueryMatches(index, "thomas -red");
    EXPECT_EQUAL(matchesRedOrFish.size(), 0);
    Set<string> matchesRedAndFish = findQueryMatches(index, "red -fish");
    EXPECT_EQUAL(matchesRedAndFish.size(), 1);
    Set<string> matchesRedWithoutFish = findQueryMatches(index, "red -friend");
    EXPECT_EQUAL(matchesRedWithoutFish.size(), 2);
}

STUDENT_TEST("findQueryMatches from tiny.txt, compound queries") {
    Map<string, Set<string>> index;
    buildIndex("res/tiny.txt", index);
    Set<string> matchesRedOrFish = findQueryMatches(index, "thomas");
    EXPECT_EQUAL(matchesRedOrFish.size(), 0);
    Set<string> matchesRedAndFish = findQueryMatches(index, "red +f");
    EXPECT_EQUAL(matchesRedAndFish.size(), 0);
    Set<string> matchesRedWithoutFish = findQueryMatches(index, "red -f");
    EXPECT_EQUAL(matchesRedWithoutFish.size(), 2);
}

STUDENT_TEST("Time opperation on website.txt") {
    Map<string, Set<string>> index;
    TIME_OPERATION("res/website.txt", buildIndex("res/website.txt", index));
}

STUDENT_TEST("Large test on website.txt") {
    Map<string, Set<string>> index;
    int nPages = buildIndex("res/website.txt", index);
    EXPECT_EQUAL(nPages, 37);
    EXPECT_EQUAL(index.size(), 5748);
    EXPECT(index.containsKey("quarter"));
}

STUDENT_TEST("buildIndex from tiny.txt, 4 pages, 12 unique tokens") {
    Map<string, Set<string>> index;
    TIME_OPERATION("tiny.txt", buildIndex("res/tiny.txt", index));
    EXPECT_EQUAL(index.size(), 12);
}

STUDENT_TEST("gatherTokens correctly cleans tokens") {
    Set<string> expected = {"hello", "world", "test123"};
    EXPECT_EQUAL(gatherTokens("Hello- WORLD-- hello)_ world ( test123"), expected);
}

STUDENT_TEST("gatherTokens for empty string") {
    Set<string> expected = {};
    EXPECT_EQUAL(gatherTokens(""), expected);
}

STUDENT_TEST("gatherTokens for duplicate words") {
    Set<string> expected = {"this", "will", "remove", "reoccuring", "words"};
    EXPECT_EQUAL(gatherTokens("This will remove reoccuring words: words words words"), expected);
}

STUDENT_TEST("cleanToken on strings digits") {
    EXPECT_EQUAL(cleanToken("12345"), "12345");
    EXPECT_EQUAL(cleanToken("-123-"), "123");
    EXPECT_EQUAL(cleanToken("@_-123-_@"), "123");
    EXPECT_EQUAL(cleanToken("-1-1--1"), "111");
}

STUDENT_TEST("cleanToken on strings containing non-alpha or not-digit") {
    EXPECT_EQUAL(cleanToken("-@@-"), "");
    EXPECT_EQUAL(cleanToken(""), "");
    EXPECT_EQUAL(cleanToken("-"), "");
    EXPECT_EQUAL(cleanToken("@##@#@##@"), "");
}

STUDENT_TEST("cleanToken on strings containing multiple words") {
    EXPECT_EQUAL(cleanToken("Hello-World"), "helloworld");
    EXPECT_EQUAL(cleanToken("HELLO WORLD"), "helloworld");
    EXPECT_EQUAL(cleanToken("-HEllO-@WorlD__@"), "helloworld");
    EXPECT_EQUAL(cleanToken("-HEllO-@ WorlD__@"), "helloworld");
}

PROVIDED_TEST("cleanToken on strings of letters and digits") {
    EXPECT_EQUAL(cleanToken("hello"), "hello");
    EXPECT_EQUAL(cleanToken("WORLD"), "world");
    EXPECT_EQUAL(cleanToken("CS106B"), "cs106b");
}

PROVIDED_TEST("cleanToken on strings containing punctuation") {
    EXPECT_EQUAL(cleanToken("/hello/"), "hello");
    EXPECT_EQUAL(cleanToken("~woRLD!"), "world");
    EXPECT_EQUAL(cleanToken("they're"), "theyre");
}

PROVIDED_TEST("cleanToken on string of only punctuation") {
    EXPECT_EQUAL(cleanToken("#$^@@.;"), "");
}

PROVIDED_TEST("gatherTokens from simple string") {
    Set<string> expected = {"go", "gophers"};
    EXPECT_EQUAL(gatherTokens("go go go gophers"), expected);
}

PROVIDED_TEST("gatherTokens correctly cleans tokens") {
    Set<string> expected = {"i", "love", "cs106b"};
    EXPECT_EQUAL(gatherTokens("I _love_ CS*106B!"), expected);
}

PROVIDED_TEST("gatherTokens from seuss, 6 unique tokens, mixed case, punctuation") {
    Set<string> tokens = gatherTokens("One Fish Two Fish *Red* fish Blue fish ** 10 RED Fish?");
    EXPECT_EQUAL(tokens.size(), 6);
    EXPECT(tokens.contains("fish"));
    EXPECT(!tokens.contains("Fish"));
}

PROVIDED_TEST("buildIndex from tiny.txt, 4 pages, 12 unique tokens") {
    Map<string, Set<string>> index;
    int nPages = buildIndex("res/tiny.txt", index);
    EXPECT_EQUAL(nPages, 4);
    EXPECT_EQUAL(index.size(), 12);
    EXPECT(index.containsKey("fish"));
}

PROVIDED_TEST("findQueryMatches from tiny.txt, single word query") {
    Map<string, Set<string>> index;
    buildIndex("res/tiny.txt", index);
    Set<string> matchesRed = findQueryMatches(index, "red");
    EXPECT_EQUAL(matchesRed.size(), 2);
    EXPECT(matchesRed.contains("www.dr.seuss.net"));
    Set<string> matchesHippo = findQueryMatches(index, "hippo");
    EXPECT(matchesHippo.isEmpty());
}

PROVIDED_TEST("findQueryMatches from tiny.txt, compound queries") {
    Map<string, Set<string>> index;
    buildIndex("res/tiny.txt", index);
    Set<string> matchesRedOrFish = findQueryMatches(index, "red fish");
    EXPECT_EQUAL(matchesRedOrFish.size(), 4);
    Set<string> matchesRedAndFish = findQueryMatches(index, "red +fish");
    EXPECT_EQUAL(matchesRedAndFish.size(), 1);
    Set<string> matchesRedWithoutFish = findQueryMatches(index, "red -fish");
    EXPECT_EQUAL(matchesRedWithoutFish.size(), 1);
}
