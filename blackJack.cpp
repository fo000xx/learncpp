#include <iostream>
#include <array>
#include <random>
#include <chrono>
#include <algorithm>

//two classes and a struct, defining a deck of cards.
enum class CardSuit
{
    clubs,
    diamonds,
    hearts,
    spades,

    max_suits,
};

enum class CardRank
{
    rank_2,
    rank_3,
    rank_4,
    rank_5,
    rank_6,
    rank_7,
    rank_8,
    rank_9,
    rank_10,
    rank_jack,
    rank_queen,
    rank_king,
    rank_ace,

    max_rank,
};

struct Card
{
    CardSuit suit{};
    CardRank rank{};
};

using Deck = std::array<Card, 52>;
using Hand = std::vector<Card>;
using Index = Deck::size_type;

//Max score before losing
constexpr int g_maximumScore{ 21 };

//minimum score dealer has to have
constexpr int g_minimumDealerScore{ 17 };

//print the card using switch statements
void printCard(const Card& card)
{
    switch (card.rank)
    {
        case CardRank::rank_2:        std::cout << "2"; break;
        case CardRank::rank_3:        std::cout << "3"; break;
        case CardRank::rank_4:        std::cout << "4"; break;
        case CardRank::rank_5:        std::cout << "5"; break;
        case CardRank::rank_6:        std::cout << "6"; break;
        case CardRank::rank_7:        std::cout << "7"; break;
        case CardRank::rank_8:        std::cout << "8"; break;
        case CardRank::rank_9:        std::cout << "9"; break;
        case CardRank::rank_10:       std::cout << "10"; break;
        case CardRank::rank_jack:     std::cout << "J"; break;
        case CardRank::rank_queen:    std::cout << "Q"; break;
        case CardRank::rank_king:     std::cout << "K"; break;
        case CardRank::rank_ace:      std::cout << "A"; break;
        default:
            std::cout << "?"; 
            break;
    }

    switch (card.suit)
    {
        case CardSuit::clubs:         std::cout << "C"; break;
        case CardSuit::spades:        std::cout << "S"; break;
        case CardSuit::diamonds:      std::cout << "D"; break;
        case CardSuit::hearts:        std::cout << "H"; break;
        default:                 
            std::cout << "?"; 
            break;
    }
}

//create the deck
Deck createDeck()
{
    //initialise the array
    Deck deck{};
    Index index{ 0 };

    //for each rank in each suit save RS to array, where R is rank and S is suit.
    for (int suit { 0 }; suit < static_cast<int>(CardSuit::max_suits); ++suit)
    {
        for (int rank { 0 }; rank < static_cast<int>(CardRank::max_rank); ++rank)
        {
            deck[index].suit = static_cast<CardSuit>(suit);
            deck[index].rank = static_cast<CardRank>(rank);
            ++index;
        }
    }

    return deck;
}

//print the deck
template <typename T>
void printDeck(const T& deck)
{
    for (const auto& card : deck)
    {
        printCard(card);
        std::cout << ' ';
    }

    std::cout << '\n';
}

//shuffle the deck randomly
void shuffleDeck(Deck& deck)
{
    //static so it's only seeded once - moved from main into here to tidy up.
    //static std::mt19937 mt{ std::random_device{}() };  // was generating the same shuffle each time.
    static std::mt19937 mt{ static_cast<std::mt19937::result_type>(std::time(nullptr)) };

    std::shuffle(deck.begin(), deck.end(), mt);
}

int getCardValue(const Card& card)
{
    switch (card.rank)
    {
        case CardRank::rank_2:        return 2;
        case CardRank::rank_3:        return 3;
        case CardRank::rank_4:        return 4;
        case CardRank::rank_5:        return 5;
        case CardRank::rank_6:        return 6;
        case CardRank::rank_7:        return 7;
        case CardRank::rank_8:        return 8;
        case CardRank::rank_9:        return 9;
        case CardRank::rank_10:       return 10;
        case CardRank::rank_jack:     return 10;
        case CardRank::rank_queen:    return 10;
        case CardRank::rank_king:     return 10;
        case CardRank::rank_ace:      return 11;
        default:
            return 0;
    } 
}

int playerHandTurn(int pos, Hand& hand, Deck& deck)
{
    //set the initial score to 0 and initialize the player choice
    int playerScore{ 0 };
    char playerChoice{};
    
    //loop below deals one card at a time. Start with the first card then hit the loop
    //places current card to the back of the players hand
    hand.push_back(deck[pos]);
    //calculate score -> pos-1 as pos starts at 1.
    playerScore += getCardValue(hand[pos-1]);
    ++pos;

    //iterate through the deck and deal the ith card to the player. If player stands return to playBlackJack
    for (pos; pos < deck.size(); ++pos)
    {            
        hand.push_back(deck[pos]);
        playerScore += getCardValue(hand[pos-1]);

        //print the current hand and score and offer the player a choice to hit or stand  
        //if they hit and go bust, return their score         
        std::cout << "Player's hand: ";
        printDeck(hand);
        std::cout << "Player Score: " << playerScore << '\n';
        if (playerScore > g_maximumScore)
        {
            std::cout << "Bust!\n";
            return playerScore;
        }

        //else continue with the hit/stand offer, if stand - return their score
        std::cout << "Hit (H) or Stand (S)? ";
        std::cin >> playerChoice;
        if (playerChoice == 'S')
        {
            return playerScore;
        }
    }

    return playerScore;
}

int dealerHandTurn(int pos, Hand& hand, Deck& deck)
{
    //set the initial score to 0 and initialize the player choice
    int playerScore{ 0 };
    
    //iterate through the deck and deal the ith card to the player. If player stands return to playBlackJack
    for (pos; pos < deck.size(); ++pos)
    {            
        //places current card to the back of the players hand
        hand.push_back(deck[pos]);
        //calculate score -> pos-1 as pos starts at 1.
        playerScore += getCardValue(hand[pos-1]);

        //print the current hand and score and offer the player a choice to hit or stand  
        //if they hit and go bust, return their score         
        std::cout << "Dealer's hand: ";
        printDeck(hand);
        std::cout << "Dealer Score: " << playerScore << '\n';
        if (playerScore >= g_minimumDealerScore)
        {
            return playerScore;
        }
    }

    return playerScore;
}

bool playBlackjack(Deck& deck)
{
    //first card to house, second and third to player
    //integer to track the current dealt card position
    int deckPos{ 0 };
    Hand playerHand{};
    Hand dealerHand{};

    //first card to the dealer
    dealerHand.push_back(deck[deckPos]);
    ++deckPos;
    std::cout << "Dealer's first card: ";
    printDeck(dealerHand);

    //player plays
    int playerScore { playerHandTurn(deckPos, playerHand, deck) };
    
    //dealer plays
    int dealerScore { dealerHandTurn(deckPos, dealerHand, deck) };
    
    if (playerScore > g_maximumScore || ((dealerScore < g_maximumScore) && (dealerScore > playerScore)))
    {
        return false;
    }
    else
        return true;
}

int main()
{
    //create the deck
    auto deck { createDeck() };

    //shuffle the deck
    shuffleDeck(deck);

    bool isPlayerWinner{ playBlackjack(deck) };

    if (isPlayerWinner)
        std::cout << "Player Wins!\n";
    else
        std::cout << "Dealer Wins!\n";

    return 0;
}