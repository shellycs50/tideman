#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
} pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    // get index of candidate, if name not in candidates, return false;
    bool found = false;
    int index_of_candidate;
    for (int i = 0; i < candidate_count; i++)
    {
        if (strcmp(candidates[i], name) == 0)
        {
            index_of_candidate = i;
            found = true;
            break;
        }
    }

    if (!found)
    {
        return false;
    }

    ranks[rank] = index_of_candidate;
    return true;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = i + 1; j < candidate_count; j++)
        {
            if (!preferences[ranks[i]][ranks[j]])
            {
                preferences[ranks[i]][ranks[j]] = 1;
            }
            else
            {
                preferences[ranks[i]][ranks[j]] += 1;
            }
        }
    }
    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    // TODO
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = i; j < candidate_count; j++) // here j inits to i to avoid creating duplicates of the same pair
        {
            if (i == j)
            {
                continue;
            }
            if (preferences[i][j] > preferences[j][i])
            {
                pairs[pair_count].winner = i;
                pairs[pair_count].loser = j;
                pair_count++;
            }
            else if (preferences[i][j] < preferences[j][i])
            {
                pairs[pair_count].winner = j;
                pairs[pair_count].loser = i;
                pair_count++;
            }
            else
            {
                // ITS A DRAW
                // dont do anything - block here for readability
            }
        }
    }
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    // bubble sort
    for (int i = 0; i < pair_count - 1; i++)
    {
        for (int j = 0; j < pair_count - 1 - i; j++)
        {
            if (preferences[pairs[j].winner][pairs[j].loser] < preferences[pairs[j + 1].winner][pairs[j + 1].loser])
            {
                pair middleman = pairs[j];
                pairs[j] = pairs[j + 1];
                pairs[j + 1] = middleman;
            }
        }
    }
    return;
}

bool dfs(int current, int goal)
{
    for (int j = 0; j < candidate_count; j++)
    {
        if (locked[current][j] == true)
        {
            if (j == goal)
            {
                return true;
            }
            if (dfs(j, goal) == true)
            {
                return true;
            }
        }
    }
    return false;
}

bool creates_cycle(pair start) // need handling for multiple victories from one person
{
    return dfs(start.loser, start.winner);
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    bool cycle = true;
    for (int i = 0; i < pair_count; i++)
    {
        // check if the beating ends with itself (cycle)
        //  i.e if current lock's loser beats someone who beats the current lock's winner. ITS A CYCLE.
        if (!creates_cycle(pairs[i]))
        {
            locked[pairs[i].winner][pairs[i].loser] = true;
        }
    }
    return;
}

// Print the winner of the election
void print_winner(void)
{
    // the winner is the candidate who has no one pointing at them.
    // so we could make an array of key = candidate, value = num of candidates pointing at them. then return the candidate with 0.
    int points[candidate_count];
    for (int z = 0; z < candidate_count; z++)
    {
        points[z] = 0;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            if (locked[i][j] == true)
            {
                points[j] += 1;
            }
        }
    }
    for (int p = 0; p < candidate_count; p++)
    {
        if (points[p] == 0)
        {
            printf("%s\n", candidates[p]);
        }
    }

    return;
}