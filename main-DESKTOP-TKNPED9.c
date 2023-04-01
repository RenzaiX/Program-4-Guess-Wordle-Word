//  wordleGuessSecretWord.c
//  Have the program do the guessing to discover the secret wordle word.
//
//  Author: Mykola Turchak, 11/15/22
//  System: VS Code, Windows 11
//
//  Links to wordle dictionary words at:
//    https://www.reddit.com/r/wordle/comments/s4tcw8/a_note_on_wordles_word_list/
/*  Running the program looks like:

 */
#include <stdio.h>    // for printf(), scanf()
#include <stdlib.h>   // for exit( -1)
#include <string.h>   // for strcpy
#include <assert.h>   // for assert() sanity checks
#include <ctype.h>    // for toupper()
#include <time.h>     // for time()
#include <stdbool.h>  // for bool, true, false

// Declare globals
#define WORD_LENGTH 5     // All words have 5 letters, + 1 NULL at the end when stored
//#define WORDS_FILE_NAME "wordsLarge.txt"
#define WORDS_FILE_NAME  "wordsLarge.txt"
#define MAX_NUMBER_OF_WORDS 12947   // Number of words in the full set of words file
#define true 1   // Make boolean logic easier to understand
#define false 0  // Make boolean logic easier to understand

typedef struct wordCount wordCountStruct;
struct wordCount{
    char word[ WORD_LENGTH + 1];   // The word length plus NULL
    int score;                     // Score for the word
};


//-----------------------------------------------------------------------------------------
// Read in words from file into array.  We've previously read the data file once to
// find out how many words are in the file.
void readWordsFromFile(
            char fileName[],        // Filename we'll read from
            wordCountStruct *words, // Array of words where we'll store words we read from file
            int *wordCount)          // How many words.  Gets updated here and returned
{
    FILE *inFilePtr  = fopen(fileName, "r");  // Connect logical name to filename
    assert( inFilePtr != NULL);               // Ensure file open worked correctly

    // Read each word from file and store into array, initializing the score for that word to 0.
    char inputString[ 6];
    *wordCount = 0;
    while( fscanf( inFilePtr, "%s", inputString) != EOF) {
        strcpy( words[ *wordCount].word, inputString);
        words[ *wordCount].score = 0;
        (*wordCount)++;
    }

    // Close the file
    fclose( inFilePtr);
} // end readWordsFromFile(..)


//-----------------------------------------------------------------------------------------
// Comparator for use in built-in qsort(..) function.  Parameters are declared to be a
// generic type, so they will match with anything.
// This is a two-part comparison.  First the scores are compared.  If they are the same,
// then the words themselves are also compared, so that the results are in descending
// order by score, and within score they are in alphabetic order.
int compareFunction( const void * a, const void * b) {
    // Before using parameters we have cast them into the actual type they are in our program
    // and then extract the numerical value used in comparison
    int firstScore = ((wordCountStruct *) a)->score;
    int secondScore = ((wordCountStruct *) b)->score;

    // If scores are different, then that's all we need for our comparison.
    if (firstScore != secondScore) {
        // We reverse the values, so the result is in descending vs. the otherwise ascending order
        // return firstScore - secondScore;   // ascending order
        return secondScore - firstScore;      // descending order
    }
    else {
        // Scores are equal, so check words themselves, to put them in alphabetical order
        return strcmp( ((wordCountStruct *)a)->word,  ((wordCountStruct *)b)->word );
    }
} //end compareFunction(..)


// Check if the word is in the list of words
bool wordInList( char word[], wordCountStruct *words, int wordCount) {
    // Loop through the list of words, looking for a match and return true if found
    for( int i = 0; i < wordCount; i++) {
        if( strcmp( word, words[ i].word) == 0) {
            return true;
        }
    }
    return false;
}


// Check for matches between the guess and the secret word
void findMatch(char computerGuessCopy[], char secretWordCopy[], char displayWrongPos[]) {
    // Check for matches and if the position and letter matches change the letter to uppercase. 
    for(int i = 0; i < WORD_LENGTH; i++) 
    {
        if(computerGuessCopy[i] == secretWordCopy[i]) {
            secretWordCopy[i] = ' ';
            computerGuessCopy[i] = toupper(computerGuessCopy[i]);
        }
    }
    // If only the letter matches add asterisk to the array.
    for(int i = 0; i < WORD_LENGTH; i++) {
        for(int j = 0; j < WORD_LENGTH; j++) {
            if(computerGuessCopy[i] == secretWordCopy[j]) {
                displayWrongPos[i] = '*';
                secretWordCopy[j] = ' ';
            }
        }
    }
}

// printing the guess information
void printGuess(char computerGuessCopy[], char displayWrongPos[], int guessNumber) 
{
    // Display the guess
        printf("%5d. ", guessNumber);                         //     #. 

        for(int i = 0; i < WORD_LENGTH; i++) {  
            printf("%c ", computerGuessCopy[i]);              //        g u e s s
        }
        printf("\n       ");

        for(int i = 0; i < WORD_LENGTH; i++) {
            printf("%c ", displayWrongPos[i]);                //        * * * * *
        }

        printf("\n");
}



// Delete letters that are not in the secret word
void letterNotInWord(wordCountStruct allWords[], char computerGuessCopy[], char secretWordCopy[], char displayWrongPos[], int wordCount)
{
   bool isPreviouslyFound = false;

    for(int i = 0; i < WORD_LENGTH; i++) 
    {   
        // look for letter that are not in indicated to be in the word
        if((!isupper(computerGuessCopy[i])) && displayWrongPos[i] == ' ') 
        {
            isPreviouslyFound = false;
            char incorrectLetter = computerGuessCopy[i];
            int index = i;
            // loop trough the guess and check if the letter is not a duplicate and actually is not in the word
            for(int j = 0; j < WORD_LENGTH; j++) 
            {
                if((toupper(incorrectLetter) == computerGuessCopy[j]) || 
                   (incorrectLetter == computerGuessCopy[j] &&  displayWrongPos[j] == '*')) 
                {
                    isPreviouslyFound = true;

                } 
            }

            // delete words with the letter that have 
            for(int j = 0; j < wordCount; j++) {
                if(allWords[j].word[index] == incorrectLetter) 
                {
                    strcpy(allWords[j].word , " ");
                } 
            }
        
            // if the letter is not previously found in the word then dellete words that contain the letter
            if(!isPreviouslyFound) 
            {
                for(int j = 0; j < wordCount; j++) 
                {
                    for(int k = 0; k < WORD_LENGTH; k++) 
                    {
                        if(allWords[j].word[k] == incorrectLetter) 
                        {
                            strcpy(allWords[j].word , " ");
                        } 
                    }
                }
            }
        }
    }
}

// Delete letters that don't match the letter in computer guess at any position
void incorrectPositionLetter(wordCountStruct allWords[], char displayWrongPos[], char computerGuessCopy[], int wordCount)
{
    bool found = false;
        for(int i = 0; i < WORD_LENGTH; i++)
        {
            // check words that have the letter in the wrong position
            if(displayWrongPos[i] == '*') 
            {
                int index = i;
                char incorrectPos = computerGuessCopy[i];


                for(int j = 0; j < wordCount; j++) 
                {
                    // delete words with "incorrectPos" letter in the position it is right now
                    if(allWords[j].word[index] == incorrectPos) {
                        strcpy(allWords[j].word , " ");
                    } 
                    for(int k = 0; k < WORD_LENGTH; k++) 
                    {
                        // save words that have "incorrectPos" letter in them
                        if(allWords[j].word[k] == incorrectPos) 
                        {
                           found = true;
                        } 
                    }

                    // if letter is not to be found, delete the word from the array
                    if(!found) 
                    {
                        strcpy(allWords[j].word , " ");
                    }
                    // reset found
                    found = false;
                }
            }
        }
}

// Delete letters that don't match the position of uppercase letter in computer guess
void correctPositionLetter(wordCountStruct allWords[], char computerGuessCopy[], int wordCount)
{   
    for(int i = 0; i < WORD_LENGTH; i++) 
    {
        // look for the uppercase letters in the guess, which indicates that the letter is in the correct position
        if(isupper(computerGuessCopy[i])) 
        {
            int index = i;
            char exactPos = (computerGuessCopy[i]);
            // printf("exactPos: %c", tolower(exactPos));
            for(int j = 0; j < wordCount; j++) {
                for(int k = 0; k < WORD_LENGTH; k++) 
                {
                    // delete letters that don't have letter in the correct position
                    if(allWords[j].word[k] != tolower(exactPos) && k == index ) 
                    {
                        strcpy(allWords[j].word , " ");
                    }


                }

            }
        }
    }
}


// Used to select next guess word
void selectNextWord(wordCountStruct allWords[], char computerGuess[], int wordCount, int guessNumber) 
{
    // pre-select the second word for a guess
    if(guessNumber == 1)
    {
        strcpy(computerGuess, "mends");
        return;
    }
    // pre-select the third word for a guess
    if(guessNumber == 2)
    {
        strcpy(computerGuess, "lucky");
        return;
    }

    // select a word starting from guess number 3
    for(int i = 0; i < wordCount; i++) 
    {
        // check if the word is not empty
        if(allWords[i].word[0] != ' ' ) 
        {
            strcpy(computerGuess, allWords[i].word);
            break;
        }
    }
}


// -----------------------------------------------------------------------------------------
// Find a secret word
void findSecretWord(
        wordCountStruct allWords[],    // Array of all the words
        int wordCount,                  // How many words there are in allWords
        char secretWord[])              // The word to be guessed
{
    char computerGuess[ 6];  // Allocate space for the computer guess
    char computerGuessCopy[ 6];
    char secretWordCopy[ 6];
    char displayWrongPos[6];
    printf("Trying to find secret word: \n");
    // Display secret word with a space between letters, to match the guess words below.
    printf("       ");
    for( int i=0; i<WORD_LENGTH; i++) {
        printf("%c ", secretWord[ i]);
    }
    printf("\n");
    printf("\n");

    // Hard code the first guess
    strcpy(computerGuess, "ratio");

    // Check if word exist
    if(!(wordInList(secretWord, allWords, wordCount)))
    {
        printf("Word not in list! Try again!\n");
        exit(0);
    }

    // Loop until the word is found
    int guessNumber = 1;
    while( true) 
    {
        // adjust variables
        strcpy(computerGuessCopy, "     ");
        strcpy(secretWordCopy, "     ");
        strcpy(displayWrongPos, "     ");
        strcpy(computerGuessCopy, computerGuess);
        strcpy(secretWordCopy, secretWord);

        // Check for matches between the guess and the secret word
        findMatch(computerGuessCopy, secretWordCopy, displayWrongPos);

        // display the guess
        printGuess(computerGuessCopy, displayWrongPos, guessNumber);  

        // Check if the guess is correct
        if(strcmp(computerGuess, secretWord) == 0) 
        {
            printf("Found secret word in %d guesses. %s\n", guessNumber, secretWord);
            break;
        }

        // Delete words that don't match with guessed information about secret word
        letterNotInWord(allWords, computerGuessCopy, secretWordCopy, displayWrongPos, wordCount);
        incorrectPositionLetter(allWords, displayWrongPos, computerGuessCopy, wordCount);
        correctPositionLetter(allWords, computerGuessCopy, wordCount);

        // Select next guess word
        selectNextWord(allWords, computerGuess, wordCount, guessNumber);

        // Increment guess number
        guessNumber++;
    } //end for( int i...)
} //end findSecretWord




// -----------------------------------------------------------------------------------------
int main() {
    char wordsFileName[81];                   // Stores the answers file name
    strcpy(wordsFileName, WORDS_FILE_NAME);   // Set the filename, defined at top of program.
    srand( (unsigned) time( NULL));           // Seed the random number generator to be current time
    // Declare space for all the words, of a maximum known size.
    wordCountStruct allWords[ MAX_NUMBER_OF_WORDS];
    // Start out the wordCount to be the full number of words.  This will decrease as
    //    play progresses each time through the game.
    int wordCount = 0;
    // The secret word that the computer will try to find, plus the return character from fgets.
    char secretWord[ WORD_LENGTH + 1];
    char userInput[ 81];                // Used for menu input of secret word

    // Read in words from file, update wordCount and display information
    readWordsFromFile( wordsFileName, allWords, &wordCount);
    printf("Using file %s with %d words. \n", wordsFileName, wordCount);

    // Run the word-guessing game three times
    for( int i=0; i< 3; i++) {
        // Reset secret Word
        strcpy( secretWord, "");
        // Display prompt
        printf("-----------------------------------------------------------\n");
        printf("\n");
        printf("Enter a secret word or just r to choose one at random: ");
        scanf(" %s", userInput);


        wordCountStruct allWordsCopy[ MAX_NUMBER_OF_WORDS];
        for(int i = 0; i < wordCount; i++) {
            allWordsCopy[i] = allWords[i];
        }
        // Eliminate the return character at end or userInput if it is there
        int length = (int) strlen( userInput);
        if( userInput[ length] == '\n') {
            userInput[ length] = '\0';
        }
        strcpy( secretWord, userInput);   // Store the secret word from user input
                
        // If input was 'r' then choose a word at random.
        if( strlen( secretWord) <= 1) {
            // Randomly select a secret word to be guessed.
            int randomIndex = rand() % wordCount;
            strcpy( secretWord, allWordsCopy[ randomIndex].word);
        }
        
        // Run the game once with the current secret word
        findSecretWord( allWordsCopy, wordCount, secretWord);
    }

    printf("Done\n");
    printf("\n");
    return 0;
} // end main()