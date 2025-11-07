# C++ URL Shortener with GUI

This project is a desktop URL shortener application built in C++. It features a `wxWidgets` GUI for user interaction and a lightweight `crow.h` web server to handle URL redirections.

The core of the application uses a custom `HashMap` data structure to store and retrieve short-codes and their corresponding long URLs. The application is multithreaded, running the GUI and the web server concurrently.

## 🚀 Features

* **GUI Interface:** A simple and clean user interface built with `wxWidgets`.
* **Custom Aliases:** Users can provide an optional custom "brand name" for their short URL.
* **Auto-Generation:** If no alias is provided, a unique 6-character short code is automatically generated.
* **Web Server Backend:** A `crow` web server runs on `http://localhost:8080` to handle redirection from short URLs to the original long URLs.
* **Persistent Storage:** The `HashMap` of URLs is automatically saved to `urls.txt` when the application closes and reloaded when it starts.
* **Thread Safety:** A `std::mutex` is used to ensure safe concurrent access to the `HashMap` from both the GUI thread and the server thread.

## 🛠️ How It Works

1.  **Application Start:**
    * The `MyApp` class initializes the `wxWidgets` application.
    * It reads any existing URLs from `urls.txt` into the global `HashMap` instance.
    * It launches the `StartServer()` function in a separate `std::thread`.
    * The `URLShortenerForm` GUI is created and displayed.

2.  **Web Server (`crow`)**:
    * The server runs in a detached thread and listens on port 8080.
    * It handles requests to `http://localhost:8080/<shortCode>`.
    * It performs a thread-safe lookup in the `HashMap`.
    * If found, it returns a `302 Redirect` to the corresponding long URL.
    * If not found, it returns a `404 Not Found`.

3.  **GUI (`wxWidgets`)**:
    * Users can enter a long URL and an optional alias.
    * **Validation:** The application checks if the URL is valid (starts with `http://` or `https://`) and if a custom alias is already in use.
    * **Shortening:** On "Shorten" click, the (alias, longURL) pair is inserted into the `HashMap` in a thread-safe manner.
    * **Reset:** The "Reset" button clears all text fields.

4.  **Application Close:**
    * When the user closes the GUI window, a confirmation dialog appears.
    * If confirmed, the application writes the entire content of the `HashMap` back to `urls.txt` before closing.

## 💻 Dependencies & Build

This project was built using **Visual Studio 2022** and relies on the following libraries:

* **[wxWidgets](https://www.wxwidgets.org/):** For the cross-platform GUI.
* **[Crow](https://crowcpp.org/):** A C++ micro web framework (used as `crow.h`).
* **[C++ Standard Library](https://en.cppreference.com/w/):** `thread` and `mutex` for concurrency.

To build the project, you must:
1.  Have Visual Studio with the "Desktop development with C++" workload installed.
2.  Install `wxWidgets` and set the `WXWIN` environment variable to its directory.
3.  Include `crow.h` in the project's include path.
4.  Open `DSProjectFinal.sln` and build the solution.

## 📋 How to Use

1.  Run the compiled `DSProjectFinal.exe`.
2.  The GUI window and a background web server will start.
3.  Enter a valid long URL (e.g., `https://www.google.com`) into the "Enter URL" box.
4.  (Optional) Enter a custom alias (e.g., `my-search`) into the "Brand Name" box.
5.  Click the "Shorten URL" button.
6.  The "Shortened URL" box will display the new link (e.g., `http://localhost:8080/my-search`).
7.  You can now use this link in any browser on your machine to be redirected to the original URL.
