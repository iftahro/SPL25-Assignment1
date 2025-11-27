#include "MixingEngineService.h"
#include <iostream>
#include <memory>

MixingEngineService::MixingEngineService(): decks(), active_deck(1), auto_sync(false), bpm_tolerance(0)
{
    decks[0] = nullptr;
    decks[1] = nullptr;
    std::cout << "[MixingEngineService] Initialized with 2 empty decks." << std::endl; 
}

MixingEngineService::~MixingEngineService() {
    std::cout << "[MixingEngineService] Cleaning up decks..." << std::endl;
    for (size_t i = 0; i < 2; ++i) {
        delete decks[i];
        decks[i] = nullptr;
    }
}

/**
 * @param track: Reference to the track to be loaded
 * @return: Index of the deck where track was loaded, or -1 on failure
 */
int MixingEngineService::loadTrackToDeck(const AudioTrack& track) {
    std::cout << "\n=== Loading Track to Deck ===" << std::endl;
    PointerWrapper<AudioTrack> clone = track.clone();
    if (!clone) {
        std::cerr << "[ERROR] Track: \"" << track.get_title() << "\" failed to clone" << std::endl;
        return -1;
    }
    int target = 1 - active_deck;
    std::cout << "[Deck Switch] Target deck: " << target << std::endl;
    
    clone->load();
    clone->analyze_beatgrid();

    if (decks[active_deck] != nullptr && auto_sync)
    {
        if (!can_mix_tracks(clone)) {
            sync_bpm(clone);
        }
    }
    AudioTrack* track_ptr = clone.release();
    decks[target] = track_ptr;
    std::string new_title = track_ptr->get_title();
    std::cout << "[Load Complete] \"" << new_title <<
        "\" is now loaded on deck " << target << std::endl;

    if (decks[active_deck] != nullptr) {
        std::cout << "[Unload] Unloading previous deck " << active_deck <<
            " (" << decks[active_deck]->get_title() << ")" << std::endl;
        delete decks[active_deck];
        decks[active_deck] = nullptr;
    }
    active_deck = target;
    std::cout << "[Active Deck] Switched to deck " << target << std::endl;
    return target;
}

/**
 * @brief Display current deck status
 */
void MixingEngineService::displayDeckStatus() const {
    std::cout << "\n=== Deck Status ===\n";
    for (size_t i = 0; i < 2; ++i) {
        if (decks[i])
            std::cout << "Deck " << i << ": " << decks[i]->get_title() << "\n";
        else
            std::cout << "Deck " << i << ": [EMPTY]\n";
    }
    std::cout << "Active Deck: " << active_deck << "\n";
    std::cout << "===================\n";
}

/**
 * 
 * Check if two tracks can be mixed based on BPM difference.
 * 
 * @param track: Track to check for mixing compatibility
 * @return: true if BPM difference <= tolerance, false otherwise
 */
bool MixingEngineService::can_mix_tracks(const PointerWrapper<AudioTrack>& track) const {
    if (decks[active_deck] == nullptr) return false;
    if (!track) return false;
    int active_bpm = decks[active_deck]->get_bpm();
    int new_bpm = track->get_bpm();
    int diff = std::abs(active_bpm - new_bpm); 
    return diff <= bpm_tolerance;
}

/**
 * @param track: Track to synchronize with active deck
 */
void MixingEngineService::sync_bpm(const PointerWrapper<AudioTrack>& track) const {
    if (decks[active_deck] != nullptr && track) {
        int original_bpm = track->get_bpm();
        int new_bpm = (decks[active_deck]->get_bpm() + original_bpm) / 2;
        track->set_bpm(new_bpm);
        std::cout << "[Sync BPM] Syncing BPM from " << original_bpm <<
            " to " << new_bpm << std::endl;
    }
}
