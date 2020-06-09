#include "bootloader.h"

static SPISlave slave(PB_15, PB_14, PB_13, PB_12);
static EventQueue *queue = mbed_event_queue();

void await_data()
{
        queue->event(process_data);
}

void init_slave()
{
        slave.format(8, 0);
        slave.frequency(1000000);
}

void process_data()
{
        printf("Begin process...\r\n");
        slave.reply(0x00);
        // if (slave.receive() == 0) {
        //         while (slave.receive() == 0)
        //                 ;
        // }
        char byte = 0;
        std::stringstream file(std::ios_base::in | std::ios_base::out
                               | std::ios_base::binary);
        printf("Receiving file.\r\n");
        int cycles_passed = 0;
        while (1) {
                if (slave.receive()) {
                        byte = (char)slave.read();
                        printf("%c\r\n", byte);
                        file << byte;
                        slave.reply(0x00);
                        cycles_passed = 0;
                } else
                        cycles_passed++;

                if (cycles_passed == 100)
                        break;
        }
        printf("Received file.\r\n");
        update_application(file, POST_APPLICATION_ADDR);
}

int update_application(std::stringstream &data, uint32_t address)
{
        // if (!data.is_open()) {
        //         printf("Failed to read file!\r\n");
        //         return -1;
        // }
        FlashIAP flash;
        data.ignore(std::numeric_limits<std::streamsize>::max());
        std::streamsize length = data.gcount();
        data.clear(); //  Since ignore will have set eof.
        data.seekg(0, std::ios_base::beg);
        printf("Filesize is: %d\r\n", length);
        flash.init();

        const uint32_t page_size = flash.get_page_size();
        char *page_buffer = new char[page_size];
        uint32_t addr = address;
        uint32_t next_sector = addr + flash.get_sector_size(addr);
        bool sector_erased = false;
        size_t size_read = 0;

        while (true) {
                memset(page_buffer, 0, sizeof(char) * page_size); // read page
                data.read(page_buffer, page_size);
                size_read = data.gcount();
                if (size_read == 0)
                        break;

                if (!sector_erased) {
                        flash.erase(addr,
                                    flash.get_sector_size(addr)); // erase page
                        sector_erased = true;
                }
                flash.program(page_buffer, addr, page_size);

                addr += page_size;
                if (addr >= next_sector) {
                        next_sector = addr + flash.get_sector_size(addr);
                        sector_erased = false;
                }
        }
        printf("Flashed 100%%\r\n");

        delete[] page_buffer;

        flash.deinit();
        return 0;
}
