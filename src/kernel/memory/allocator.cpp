#include "allocator.h"
#include <kernel_layout.h>
#include <libc.h>
#include <logging.h>
#include <utils/utils.h>

namespace Memory
{

struct page_data_t
{
  u64 start;
  u16 pages_allocated;
};

static const u64 n_pages = 1024 * 64;
page_data_t page_data[n_pages];

// TODO: Can make things a bit faster for larger requests later by using a bitmap
// static const u64 n_bitmap_entry_bits = 64;
// static const u64 n_bitmap_entries = n_pages / n_bitmap_entry_bits;
// u64 page_allocation_bitmap[n_bitmap_entries];

u8* allocator_start;

void init(SMAP_entry* /*smap_data*/)
{
  // Initially all pages are uninitialized
  memset(page_data, 0, sizeof(page_data));
  // memset(page_allocation_bitmap, 0, sizeof(page_allocation_bitmap));

  allocator_start = (u8*)KERNEL_DYNAMIC;
}

int find_n_free_contiguous_pages_index(size_t requested_pages)
{
  size_t free_page_counter = 0;
  for (size_t page_i = 0; page_i < n_pages; page_i++)
  {
    if (page_data[page_i].pages_allocated != 0)
      free_page_counter = 0;
    else
      free_page_counter++;

    if (free_page_counter == requested_pages)
    {
      // The last [free_page_counter] entries (including this one), make up a big enough block.
      return page_i - requested_pages + 1;
    }
  }

  return -1;
}

void* kmalloc(size_t request_size)
{
  // You can't allocate less than a page. Round up the request to the next multiple of 4096 bytes (1 page)
  const u32 rounded_request_size = ((request_size + 4096 - 1) & ~((u64)0xFFF));
  const int requested_pages = rounded_request_size / 4096;

  const int free_area = find_n_free_contiguous_pages_index(requested_pages);
  if (free_area >= 0)
  {
    void* allocation_start = allocator_start + free_area * 4096;
    for (int page = free_area; page < free_area + requested_pages; ++page)
    {
      page_data[page].pages_allocated = requested_pages;
      page_data[page].start = (u64)allocation_start;
      // printf("Alloc'd page @ 0x%p\n", allocator_start + page*4096);
    }
    return allocation_start;
  }
  else
  {
    kpanic("Ran out of memory. Couldn't find a large enough space in the page bitmap");
  }
}

void kfree(void* ptr)
{
  if ((u8*)ptr < allocator_start)
    kpanic("pointer freed before allocator start");

  const int start_page = ((u8*)ptr - allocator_start) / 4096;
  const int allocated_pages = page_data[start_page].pages_allocated;

  if (allocated_pages == 0)
  {
    kpanic("Free of an un-allocated page(s)\n");
  }

  for (int page = start_page; page < start_page + allocated_pages; ++page)
  {
    page_data[page].pages_allocated = 0;
    page_data[page].start = 0xDEADDEADDEADDEAD;
    // printf("Freed page @ 0x%p\n", allocator_start + page*4096);
  }
}

} // namespace Memory