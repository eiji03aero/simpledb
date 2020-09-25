#include <iostream>
#include <cstring>

using namespace std;

const int page_size = 4;

class Page {
public:
  char *content_;

  Page() {
    content_ = new char[page_size];
  }

  ~Page() {
    delete[] content_;
  }
};

int main()
{
  Page *page { new Page };

  int a {4};
  int b {8};

  int a2;
  int b2;

  char* content_ptr {page->content_};

  memcpy(page->content_, &a, sizeof(int));
  memcpy(page->content_ + sizeof(int), &b, sizeof(int));

  memcpy(&a2, page->content_, sizeof(int));
  memcpy(&b2, page->content_ + sizeof(int), sizeof(int));

  cout << a << endl; // 4
  cout << b << endl; // 8
  cout << a2 << endl; // 4
  cout << b2 << endl; // 8

  delete page;

  memcpy(&a2, content_ptr, sizeof(int));
  memcpy(&b2, content_ptr + sizeof(int), sizeof(int));

  cout << a2 << endl; // 0
  cout << b2 << endl; // 0

  return 0;
}
