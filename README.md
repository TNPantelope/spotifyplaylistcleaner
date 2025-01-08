# Spotify Playlist Cleaner

## Introduction
Ever tried to look for a playlist to match a certain vibe, or a playlist within a specific genre, just for half the songs in the playlists to be something you've already listened to to death?
Same, this is a tool that removes liked songs from playlists. Unfortunately this does not work with playlists made by Spotify themselves, only user-generated ones.

## Features
- Removes songs you've already liked from any user-generated playlist
- Creates a copy of the playlist, doesn't modify the original one
- Works cross-platform (Windows and Mac)

## Prerequisites
- C++ compiler 
- CMake 
- OpenSSL
- Spotify Developer Account
- cpprestsdk 

## Windows Setup
1. Install Visual Studio 2022 with C++ desktop development
2. Install CMake from [cmake.org/download](https://cmake.org/download/)
3. Install OpenSSL from [slproweb.com](https://slproweb.com/products/Win32OpenSSL.html) (Win64 version)
4. Install vcpkg:
   ```
   cd C:\<makethisfolder>
   git clone https://github.com/Microsoft/vcpkg.git
   cd vcpkg
   ./bootstrap-vcpkg.bat
   ./vcpkg install cpprestsdk:x64-windows
   ```


## Mac Setupo
1. Install Xcode Command Line Tools:
    xcode-select --install 
2. Install dependencies using Homebrew:
    brew install cmake openssl cpprestsdk

## Usage
1. Get the playlist ID you want to clean:
- Open the playlist in Spotify
- The ID is the string after `playlist/` in the URL
- Example: For `https://open.spotify.com/playlist/7axCy8ti9svTLdhZogIKqo?si=...`
- The ID would be `7axCy8ti9svTLdhZogIKqo`

2. Run the program:
- Enter your Spotify API Client ID when prompted
- Enter your Client Secret when prompted
- Click the authentication link and log into Spotify
- Enter the playlist ID you want to clean
- Enter a name for your new playlist

3. The program will:
- Fetch all your liked songs
- Get all songs from the target playlist
- Create a new playlist without your liked songs
- Show some statistics about matched and unique songs

## Technical Notes
- The program uses OAuth 2.0 for Spotify authentication
- Creates a local server on port 8888 for OAuth callback
- Uses cpprestsdk for the HTTP server
- Uses curl for Spotify API requests
- Cross-platform compatible should work on macos and windows