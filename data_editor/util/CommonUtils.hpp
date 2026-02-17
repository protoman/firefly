//
// Created by iuri on 16/02/2026.
//

#ifndef FIREFLYEDITOR_COMMONUTILS_HPP
#define FIREFLYEDITOR_COMMONUTILS_HPP

#include <string>
#include <QComboBox>

class CommonUtils {
public:
    static CommonUtils* get_instance();
    static void fill_files_combo(std::string directory, QComboBox* combo, bool show_none=false);
    static void show_directory_error_message(std::string directory);


private:
    CommonUtils();
    CommonUtils(CommonUtils const&)= default;             // copy constructor is private
    CommonUtils& operator=(CommonUtils const&){ return *this; };  // assignment operator is private

private:
    static CommonUtils* _instance;

};


#endif //FIREFLYEDITOR_COMMONUTILS_HPP