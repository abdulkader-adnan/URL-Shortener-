#include <wx/wx.h>
#include <thread>
#include <mutex>
#include "crow.h"
#include "HashMap.h"
using namespace std;

HashMap urlMap;
mutex mapMutex;

bool isValidURL(const string& url) {
    if (url.substr(0, 7) == "http://" || url.substr(0, 8) == "https://") {
        int pos = url.find('.', 8); 
        if (pos != string::npos && pos > 8) {
            return true;
        }
    }
    return false;
}

bool isAliasUnique(const string& alias) {
    return urlMap.find(alias) == nullptr;
}

string generateRandomShortCode() {
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    const int length = 6;
    string result;
    for (int i = 0; i < length; ++i)
        result += charset[rand() % (sizeof(charset) - 1)];
    return result;
}

void StartServer() {
    crow::SimpleApp app;
    CROW_ROUTE(app, "/<string>")([](const string& shortCode) {
        lock_guard<mutex> lock(mapMutex);
        const string* it = urlMap.find(shortCode);
        if (it != nullptr) {
            crow::response res(302);
            res.add_header("Location", *it);
            return res;
        }
        return crow::response(404, "Short URL not found!");
        });
    CROW_ROUTE(app, "/add")([](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body || body["shortCode"].t() != crow::json::type::String || body["url"].t() != crow::json::type::String) {
            return crow::response(400, "Invalid input!");
        }

        string shortCode = body["shortCode"].s();
        string url = body["url"].s();

        if (!isValidURL(url)) {
            return crow::response(400, "Invalid URL format!");
        }
        {
            lock_guard<mutex> lock(mapMutex);
            if (!isAliasUnique(shortCode)) {
                return crow::response(400, "Alias already exists!");
            }
            urlMap[shortCode] = url;
        }
        return crow::response(200, "Short URL added successfully!");
        });
    app.port(8080).multithreaded().run();
}

//GUI
class URLShortenerForm : public wxFrame {
private:
    wxTextCtrl* inputTextBox;
    wxTextCtrl* aliasTextBox;
    wxTextCtrl* outputTextBox;
    wxButton* shortenButton;
    wxButton* resetButton;

public:
    URLShortenerForm() : wxFrame(NULL, wxID_ANY, "URL Shortener", wxDefaultPosition, wxSize(400, 300)) {
        wxStaticText* inputLabel = new wxStaticText(this, wxID_ANY, "Enter URL:", wxPoint(20, 20));
        inputTextBox = new wxTextCtrl(this, wxID_ANY, "", wxPoint(20, 50), wxSize(300, 20));

        wxStaticText* aliasLabel = new wxStaticText(this, wxID_ANY, "Brand Name (optional):", wxPoint(20, 80));
        aliasTextBox = new wxTextCtrl(this, wxID_ANY, "", wxPoint(20, 110), wxSize(300, 20));

        shortenButton = new wxButton(this, wxID_ANY, "Shorten URL", wxPoint(20, 140), wxSize(100, 30));
        shortenButton->Bind(wxEVT_BUTTON, &URLShortenerForm::OnShortenClick, this);

        resetButton = new wxButton(this, wxID_ANY, "Reset", wxPoint(130, 140), wxSize(100, 30));
        resetButton->Bind(wxEVT_BUTTON, &URLShortenerForm::OnResetClick, this);

        wxStaticText* outputLabel = new wxStaticText(this, wxID_ANY, "Shortened URL:", wxPoint(20, 180));
        outputTextBox = new wxTextCtrl(this, wxID_ANY, "", wxPoint(20, 210), wxSize(300, 20), wxTE_READONLY);

        Bind(wxEVT_CLOSE_WINDOW, &URLShortenerForm::OnClose, this);
    }
    void OnShortenClick(wxCommandEvent& event) {
        string longURL = inputTextBox->GetValue().ToStdString();
        string alias = aliasTextBox->GetValue().ToStdString();

        if (longURL.empty() || !isValidURL(longURL)) {
            wxMessageBox("Please enter a valid URL.", "Error", wxICON_ERROR);
            return;
        }
        {
            lock_guard<mutex> lock(mapMutex);
            if (!alias.empty() && !isAliasUnique(alias)) {
                wxMessageBox("Alias already exists!", "Error", wxICON_ERROR);
                return;
            }

            if (alias.empty()) {
                alias = to_string(urlMap.size() + 1);
                do
                {
                    alias = generateRandomShortCode();
                } while (urlMap.find(alias));
            }

            urlMap[alias] = longURL;
        }

        outputTextBox->SetValue("http://localhost:8080/" + alias);
    }
    void OnResetClick(wxCommandEvent& event) {
        inputTextBox->Clear();
        aliasTextBox->Clear();
        outputTextBox->Clear();
    }
    // Close button handler
    void OnClose(wxCloseEvent& event) {
        // Optional: Ask the user for confirmation
        if (wxMessageBox("Are you sure you want to close the application?", "Confirm",
            wxICON_QUESTION | wxYES_NO, this) == wxYES) {
            urlMap.writeHashMapToFile("urls.txt");
            Destroy(); // Close the frame

        }
        else {
            event.Veto(); // Cancel the close event
        }
    }

};

// wxWidgets Application
class MyApp : public wxApp {
public:
    virtual bool OnInit() {
        thread serverThread(StartServer); // Start the server in a separate thread
        serverThread.detach();
        urlMap.readHashMapFromFile("urls.txt");
        URLShortenerForm* frame = new URLShortenerForm();
        frame->Show(true);
        return true;
    }
};
wxIMPLEMENT_APP(MyApp);
