/*
 * Getting timelines by Twitter Streaming API
 */
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <oauth.h>
#include <curl/curl.h>
#include <sstream>

using namespace std;

size_t fncCallback(char *ptr, size_t size, size_t nmemb, string *stream)
{
  int iRealSize = size * nmemb;
  stream->append(ptr, iRealSize);
  string str = *stream;
  cout << str << endl;
  return iRealSize;
}

class Proc
{
  const char *cUrl;
  const char *cConsKey;
  const char *cConsSec;
  const char *cAtokKey;
  const char *cAtokSec;
  CURL *curl;
  char *cSignedUrl;
  string chunk;

public:
  Proc(const char *, const char *, const char *, const char *, const char *);
  void execProc();
};

// Constructor
Proc::Proc(
    const char *cUrl,
    const char *cConsKey, const char *cConsSec,
    const char *cAtokKey, const char *cAtokSec)
{
  this->cUrl = cUrl;
  this->cConsKey = cConsKey;
  this->cConsSec = cConsSec;
  this->cAtokKey = cAtokKey;
  this->cAtokSec = cAtokSec;
}

void Proc::execProc()
{
  // ==== cURL Initialization
  curl_global_init(CURL_GLOBAL_ALL);
  curl = curl_easy_init();
  if (!curl)
  {
    cout << "[ERROR] curl_easy_init" << endl;
    curl_global_cleanup();
    return;
  }

  // ==== cURL Setting
  // - URL, POST parameters, OAuth signing method, HTTP method, OAuth keys
  cSignedUrl = oauth_sign_url2(
      cUrl, NULL, OA_HMAC, "GET",
      cConsKey, cConsSec, cAtokKey, cAtokSec);
  // - URL
  curl_easy_setopt(curl, CURLOPT_URL, cSignedUrl);
  // - User agent name
  curl_easy_setopt(curl, CURLOPT_USERAGENT, "mk-mode BOT");
  // - HTTP STATUS >=400 ---> ERROR
  curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1);
  // - Callback function
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fncCallback);
  // - Write data
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

  // ==== Execute
  int iStatus = curl_easy_perform(curl);
  if (!iStatus)
    cout << "[ERROR] curl_easy_perform: STATUS=" << iStatus << endl;

  // ==== cURL Cleanup
  curl_easy_cleanup(curl);
  curl_global_cleanup();
}

int main(int argc, const char *argv[])
{
  // ==== Constants - URL
  const char *URL = "https://stream.twitter.com/1.1/statuses/sample.json";
  // ==== Constants - Twitter kyes
  const char *CONS_KEY = "<YOUR_CONSUMER_KEY>";
  const char *CONS_SEC = "<YOUR_CONSUMER_SECRET>";
  const char *ATOK_KEY = "<YOUR_ACCESS_TOKEN>";
  const char *ATOK_SEC = "<YOUR_ACCESS_TOKEN_SECRET>";

  // ==== Instantiation
  Proc objProc(URL, CONS_KEY, CONS_SEC, ATOK_KEY, ATOK_SEC);

  // ==== Main proccess
  objProc.execProc();

  return 0;
}
