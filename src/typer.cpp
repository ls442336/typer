#include <iostream>
#include <vector>
#include <ctime>
#include <fstream>
#include <sstream>

using namespace std;

class Word
{
private:
    string m_value;
    bool m_isWrong;

public:
    Word(string t_value) :m_value(t_value),  m_isWrong(false) {}

    string getValue()
    {
        return m_value;
    }

    bool isWrong()
    {
        return m_isWrong;
    }

    void validate(string t_word)
    {
        m_isWrong = !(m_value == t_word);
    }
};

class Typer
{
private:
    vector<Word> m_words;

    int m_phraseStart;
    int m_phraseEnd;
    int m_currWord;
    int m_maxCharacters;

    int m_wrongs;
    int m_rights;

public:
    Typer(int t_maxCharacters)
    {
        m_maxCharacters = t_maxCharacters;
    }

    void loadWords(string text)
    {
        string word = "";

        m_words = vector<Word>();

        for (int i = 0; i < text.size(); i++)
        {
            if (text[i] == ' ')
            {
                m_words.push_back(Word(word));
                word = "";
            }
            else if (i == text.size() - 1)
            {
                word += text[i];
                m_words.push_back(Word(word));
                word = "";
            }
            else
            {
                word += text[i];
            }
        }
    }

    void restart(){
        m_currWord = 0;
        m_phraseStart = -1;
        m_phraseEnd = -1;
        m_wrongs = 0;
        m_rights = 0;
    }

    bool hasCursorReachPhraseEnd(){
        if(m_currWord > m_phraseEnd){
            return true;
        }

        return false;
    }

    bool hasCursorReachTextEnd(){
        if(m_currWord >= m_words.size()){
            return true;
        }

        return false;
    }

    void nextPhrase()
    {
        if (m_currWord >= m_words.size())
        {
            return;
        }

        if (m_words[m_currWord].getValue().size() > m_maxCharacters)
        {
            return;
        }

        m_phraseStart = m_currWord;
        m_phraseEnd = m_currWord;

        int maxCharacters = m_words[m_currWord].getValue().size();
        for (int i = m_currWord + 1; i < m_words.size(); i++)
        {
            if (maxCharacters + m_words[i].getValue().size() <= m_maxCharacters)
            {
                maxCharacters += m_words[i].getValue().size() + 1;
                m_phraseEnd = i;
            }
            else
            {
                break;
            }
        }
    }

    void resizePhrase(int t_maxCharacters)
    {
        m_maxCharacters = t_maxCharacters;

        int m = -1;

        for (int i = m_phraseStart; i <= m_phraseEnd; i++)
        {
            m += m_words[i].getValue().size() + 1;
        }

        for (int i = m_phraseEnd + 1; i <= m_words.size(); i++)
        {
            if (m + m_words[i].getValue().size() <= m_maxCharacters)
            {
                m += m_words[i].getValue().size() + 1;
                m_phraseEnd = i;
            }
            else
            {
                break;
            }
        }
    }

    bool typedWord(string t_word){
        m_words[m_currWord].validate(t_word);

        if(m_words[m_currWord].isWrong()){
            m_wrongs++;
        }else{
            m_rights++;
        }

        return !(m_words[m_currWord].isWrong());
    }

    void nextWord(){
        m_currWord++;
    }

    vector<Word> getCurrentPhrase()
    {
        vector<Word> phrase;

        for(int i = m_phraseStart; i <= m_phraseEnd; i++){
            phrase.push_back(m_words[i]);
        }

        return phrase;
    }

    int getCurrentWordIndex(){
        return m_currWord - m_phraseStart;
    }

    int getNumOfWrongWords(){
        return m_wrongs;
    }

    int getNumOfRightWords(){
        return m_rights;
    }
};

class TyperViewer{
private:
    Typer& m_typer;
public:
    TyperViewer(Typer &t_typer) : m_typer(t_typer){}

    void display(){
        vector<Word> currWords = m_typer.getCurrentPhrase();

        system("clear");

        for(int i = 0; i < currWords.size(); i++){
            if(i > m_typer.getCurrentWordIndex()){
                cout << currWords[i].getValue() << " ";
            }else if(i == m_typer.getCurrentWordIndex()){
                cout << "\033[1;37m" << currWords[i].getValue() << "\033[0m ";
            }else if(currWords[i].isWrong()){
                cout << "\033[1;31m" << currWords[i].getValue() << "\033[0m ";
            }else{
                cout << "\033[1;32m" << currWords[i].getValue() << "\033[0m ";
            }
        }

        cout << endl;
    }
};

int main(int argc, char *argv[])
{
    Typer typer(stoi(argv[1]));

    ifstream file;
    file.open("dict");
    stringstream dictstream;
    dictstream << file.rdbuf();
    file.close();

    typer.loadWords(dictstream.str());
    typer.restart();

    TyperViewer typerViewer(typer);

    time_t startTime = time(0);

    while(!typer.hasCursorReachTextEnd() && (time(0) - startTime) <  60){
        if(typer.hasCursorReachPhraseEnd()){
            typer.nextPhrase();
        }

        typerViewer.display();

        string wordTyped;
        cout << "--------------------------------------------------------------------------------" << endl;
        cout << ">> ";
        cin >> wordTyped;

        typer.typedWord(wordTyped);
        typer.nextWord();

        typerViewer.display();
    }

    time_t endTime = time(0);

    cout << "*-------Stats-------*" << endl;
    cout << "Num of wrong words " << typer.getNumOfWrongWords() << endl;
    cout << "Num of right words " << typer.getNumOfRightWords() << endl;
    cout << "Total of words " << typer.getNumOfRightWords() + typer.getNumOfWrongWords() << endl;
    cout << "wpm " << (int)(typer.getNumOfRightWords() / ((double)(endTime - startTime) / 60.0)) << endl; 

    return 0;
}
