#include "DJLibraryService.h"
#include "SessionFileParser.h"
#include "MP3Track.h"
#include "WAVTrack.h"
#include <iostream>
#include <memory>
#include <filesystem>


DJLibraryService::DJLibraryService(const Playlist& playlist) 
    : playlist(playlist), library() {}

DJLibraryService::~DJLibraryService() {
    for (AudioTrack* track : library) {
        delete track;
    }
}

/**
 * @brief Load a playlist from track indices referencing the library
 * @param library_tracks Vector of track info from config
 */
void DJLibraryService::buildLibrary(const std::vector<SessionConfig::TrackInfo>& library_tracks) {
    for (const SessionConfig::TrackInfo& track_info : library_tracks) {
        AudioTrack* new_track;
        if (track_info.type == "MP3") {
            new_track = new MP3Track(track_info.title, track_info.artists, track_info.duration_seconds,
            track_info.bpm, track_info.extra_param1, track_info.extra_param2);
        }
        else if (track_info.type == "WAV") {
            new_track = new WAVTrack(track_info.title, track_info.artists, track_info.duration_seconds,
            track_info.bpm, track_info.extra_param1, track_info.extra_param2);
        }
        library.push_back(new_track);
    }
    std::cout << "[INFO] Track library built: " << library.size() << " tracks loaded"<<std::endl;
}

/**
 * @brief Display the current state of the DJ library playlist
 * 
 */
void DJLibraryService::displayLibrary() const {
    std::cout << "=== DJ Library Playlist: " 
              << playlist.get_name() << " ===" << std::endl;

    if (playlist.is_empty()) {
        std::cout << "[INFO] Playlist is empty.\n";
        return;
    }

    // Let Playlist handle printing all track info
    playlist.display();

    std::cout << "Total duration: " << playlist.get_total_duration() << " seconds" << std::endl;
}

/**
 * @brief Get a reference to the current playlist
 * 
 * @return Playlist& 
 */
Playlist& DJLibraryService::getPlaylist() {
    // Your implementation here
    return playlist;
}

/**
 * 
 * HINT: Leverage Playlist's find_track method
 */
AudioTrack* DJLibraryService::findTrack(const std::string& track_title) {
    return playlist.find_track(track_title);
}

void DJLibraryService::loadPlaylistFromIndices(const std::string& playlist_name, 
                                               const std::vector<int>& track_indices) {
    std::cout << "[INFO] Loading playlist: " << playlist_name << std::endl;
    
    Playlist new_playlist = Playlist(playlist_name);
    playlist = new_playlist;
    
    for (int index : track_indices) {
        if (index >= (int)library.size() + 1 || index < 1) {
            std::cout << "[WARNING] Invalid track index: " << index << std::endl;
            continue;
        } 
        AudioTrack* track = library[index-1];
        PointerWrapper<AudioTrack> clone = track->clone();
        if (!clone) {
            std::cerr << "[ERROR] Track " << index-1 << " failed to clone" << std::endl;
            continue;
        }
        clone->load();
        clone->analyze_beatgrid();
        playlist.add_track(clone.release());
    }
    std::cout << "[INFO] Playlist loaded: " << playlist_name << " (" << 
        playlist.get_track_count() << " tracks)" << std::endl;
}
/**
 * @return Vector of track titles in the playlist
 */
std::vector<std::string> DJLibraryService::getTrackTitles() const {
    std::vector<std::string> titles;

    for (const AudioTrack* track : playlist.getTracks()) {
        titles.push_back(track->get_title());
    }

    return titles;
}
