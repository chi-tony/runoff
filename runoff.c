#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max voters and candidates
#define MAX_VOTERS 100
#define MAX_CANDIDATES 9

// preferences[i][j] is jth preference for voter i
int preferences[MAX_VOTERS][MAX_CANDIDATES];

// Candidates have name, vote count, eliminated status
typedef struct
{
    string name;
    int votes;
    bool eliminated;
} candidate;

// Array of candidates
candidate candidates[MAX_CANDIDATES];

// Numbers of voters and candidates
int voter_count;
int candidate_count;

// Function prototypes
bool vote(int voter, int rank, string name);
void tabulate(void);
bool print_winner(void);
int find_min(void);
bool is_tie(int min);
void eliminate(int min);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: runoff [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX_CANDIDATES)
    {
        printf("Maximum number of candidates is %i\n", MAX_CANDIDATES);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i].name = argv[i + 1];
        candidates[i].votes = 0;
        candidates[i].eliminated = false;
    }

    voter_count = get_int("Number of voters: ");
    if (voter_count > MAX_VOTERS)
    {
        printf("Maximum number of voters is %i\n", MAX_VOTERS);
        return 3;
    }

    // Keep querying for votes
    for (int i = 0; i < voter_count; i++)
    {

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            // Record vote, unless it's invalid
            if (!vote(i, j, name))
            {
                printf("Invalid vote.\n");
                return 4;
            }
        }

        printf("\n");
    }

    // Keep holding runoffs until winner exists
    while (true)
    {
        // Calculate votes given remaining candidates
        tabulate();

        // Check if election has been won
        bool won = print_winner();
        if (won)
        {
            break;
        }

        // Eliminate last-place candidates
        int min = find_min();
        bool tie = is_tie(min);

        // If tie, everyone wins
        if (tie)
        {
            for (int i = 0; i < candidate_count; i++)
            {
                if (!candidates[i].eliminated)
                {
                    printf("%s\n", candidates[i].name);
                }
            }
            break;
        }

        // Eliminate anyone with minimum number of votes
        eliminate(min);

        // Reset vote counts back to zero
        for (int i = 0; i < candidate_count; i++)
        {
            candidates[i].votes = 0;
        }
    }
    return 0;
}

// Record preference if vote is valid
bool vote(int voter, int rank, string name)
{
    // Loop through each candidate column
    for (int i = 0; i < candidate_count; i++)
    {
        // Check if vote matches any existing candidates and update preferences
        if (strcmp(candidates[i].name, name) == 0)
        {
            preferences[voter][rank] = i;
            return true;
        }
    }
    return false;
}

// Tabulate votes for non-eliminated candidates
void tabulate(void)
{
    // Loop through each voter
    for (int i = 0; i < voter_count; i++)
    {
        int j = 0;

        int candidate_index = preferences[i][j];

        // Check if first preference has been eliminated
        if (candidates[candidate_index].eliminated == false)
        {
            candidates[candidate_index].votes++;
        }

        else
        {
            // If first preference eliminated, loop through remaining preferences
            for (j = 1; j < candidate_count; j++)
            {
                // Check if candidate is eliminated and update votes, breaking loop
                if (candidates[j].eliminated == false)
                {
                    candidates[j].votes++;
                    break;
                }

                // If candidate eliminated, continue looping
                else
                {
                    continue;
                }
            }
        }
    }
    return;
}

// Print the winner of the election, if there is one
bool print_winner(void)
{
    // Define votes needed to reach majority
    int majority = voter_count / 2 + 1;

    // Loop through candidates and check if they have a majority vote
    for (int i = 0; i < candidate_count; i++)
    {
        if (candidates[i].votes >= majority)
        {
            printf("%s\n", candidates[i].name);
            return true;
        }
    }
    return false;
}

// Return the minimum number of votes any remaining candidate has
int find_min(void)
{

    int min = 0;

    // Initialize minimum as the first eligible candidate's vote count
    for (int i = 0; i < candidate_count; i++)
    {
        if (candidates[i].eliminated == false)
        {
            min = candidates[i].votes;
            break;
        }
    }

    // Loop through candidates
    for (int j = 1; j < candidate_count; j++)
    {
        // Check if the candidate has less than the minimum and has not been eliminated yet
        if (candidates[j].votes < min && candidates[j].eliminated == false)
        {
            // Set their vote count as the new minimum
            min = candidates[j].votes;
        }
    }
    return min;
}

// Return true if the election is tied between all candidates, false otherwise
bool is_tie(int min)
{
    int min_count = 0;
    int eliminated_count = 0;

    // Loop through candidates, checking if their vote count is equal to the minimum
    for (int i = 0; i < candidate_count; i++)
    {
        // Increase the counter for number of candidates with the minimum number of votes
        if (candidates[i].votes == min)
        {
            min_count++;
        }

        // Increase the counter for number of eliminated candidates
        if (candidates[i].eliminated == true)
        {
            eliminated_count++;
        }
    }

    // If there is a tie, there will be more than 1 candidate with the minimum votes
    // and the remaining number of candidates will equal the amount with the minimum
    if (min_count >= 2 & candidate_count - eliminated_count == min_count)
    {
        return true;
    }

    return false;
}

// Eliminate the candidate (or candidates) in last place
void eliminate(int min)
{
    // Loop through candidates, seeing if they have the minimum number of votes
    for (int i = 0; i < candidate_count; i++)
    {
        if (candidates[i].votes == min)
        {
            // Set their status to eliminated
            candidates[i].eliminated = true;
        }
    }
    return;
}
