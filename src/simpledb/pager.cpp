#include "simpledb/pager.h"

namespace simpledb {

void Pager::open(const char *filename) {
  file_descriptor = ::open(filename, O_RDWR | O_CREAT, S_IWUSR | S_IRUSR);

  if (file_descriptor == -1) {
    std::cout << "Error: unable to open file" << '\n';
    exit(EXIT_FAILURE);
  }

  file_length = lseek(file_descriptor, 0, SEEK_END);
  num_pages = file_length / Page::PageSize;

  if (file_length % Page::PageSize != 0) {
    std::cout << "Error: db file is not a whole number of pages. Corrupt file." << '\n';
    exit(EXIT_FAILURE);
  }

  for (int i {0}; i < MaxPages; i++) {
    pages[i] = NULL;
  }
}

Page* Pager::get_page(uint32_t page_num) {
  if (page_num > MaxPages) {
    std::stringstream ss;
    ss << "Error: tried to fetch page number out of bounds. " << page_num << " > " << MaxPages << '\n';
    exit(EXIT_FAILURE);
  }

  if (pages[page_num] == NULL) {
    Page *page { new Page };
    uint32_t l_num_pages { file_length / Page::PageSize };

    if (file_length % Page::PageSize) {
      l_num_pages += 1;
    }

    if (page_num <= l_num_pages) {
      lseek(file_descriptor, page_num * Page::PageSize, SEEK_SET);
      ssize_t bytes_read { read(file_descriptor, page->content, Page::PageSize) };
      if (bytes_read == -1) {
        std::stringstream ss;
        ss << "Error: reading file: " << errno << '\n';
        exit(EXIT_FAILURE);
      }
    }

    pages[page_num] = page;

    if (page_num >= this->num_pages) {
      this->num_pages = page_num + 1;
    }
  }

  return pages[page_num];
}

void Pager::delete_page(uint32_t page_num) {
  delete pages[page_num];
  pages[page_num] = NULL;
}

void Pager::flush(uint32_t page_num) {
  if (pages[page_num] == NULL) {
    std::cout << "Error tried to flush null page" << '\n';
    exit(EXIT_FAILURE);
  }

  off_t offset { lseek(file_descriptor, page_num * Page::PageSize, SEEK_SET) };
  if (offset == -1) {
    std::cout << "Error: seeking " << errno << '\n';
    exit(EXIT_FAILURE);
  }

  ssize_t bytes_written { write(file_descriptor, pages[page_num]->content, Page::PageSize) };
  if (bytes_written == -1) {
    std::cout << "Error: writing " << errno << '\n';
    exit(EXIT_FAILURE);
  }
}

} /* namespace simpledb */
