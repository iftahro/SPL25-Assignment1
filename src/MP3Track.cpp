#include "MP3Track.h"
#include <iostream>
#include <cmath>
#include <algorithm>

MP3Track::MP3Track(const std::string& title, const std::vector<std::string>& artists, 
                   int duration, int bpm, int bitrate, bool has_tags)
    : AudioTrack(title, artists, duration, bpm), bitrate(bitrate), has_id3_tags(has_tags) {

    std::cout << "MP3Track created: " << bitrate << " kbps" << std::endl;
}

// ========== TODO: STUDENTS IMPLEMENT THESE VIRTUAL FUNCTIONS ==========

void MP3Track::load() {
    std::cout << "[MP3Track::load] Loading MP3: \"" << title << "\" at " << bitrate << " kbps...\n";
    std::cout << (has_id3_tags ? "  → Processing ID3 metadata (artist info, album art, etc.)..." : "  → No ID3 tags found.") 
        << std::endl;
    std::cout << "  → Decoding MP3 frames..." << std::endl;
    std::cout << "  → Load complete." << std::endl;
}

void MP3Track::analyze_beatgrid() {
    std::cout << "[MP3Track::analyze_beatgrid] Analyzing beat grid for: \"" << title << "\"" << std::endl;
    double beats_estimated = (duration_seconds / 60.0) * bpm;
    double precision_factor = bitrate / 320.0;
    std::cout << "  → Estimated beats: " << beats_estimated << "  → Compression precision factor: " << precision_factor << std::endl;
}

double MP3Track::get_quality_score() const {
    double score = (bitrate / 320.0) * 100.0;
    if (has_id3_tags) score +=5;
    if (bitrate < 128) score -= 10;
    return std::max(0.0, std::min(score, 100.0));
}

PointerWrapper<AudioTrack> MP3Track::clone() const {
    return PointerWrapper<AudioTrack>(new MP3Track(*this));
}