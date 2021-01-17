#ifndef IO_H_
#define IO_H_

//! \file io.h

#include <string>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <iostream>

//!prints a message before quitting the program
/*!
\param[in] msg a string to print before exiting
*/
void print_exit (const char *msg);

//------------------------------------------------------------------------------
//string manipulation

//!removes white space at the beginning and end of a string
/*!
\param[in] s string to strip of white space
*/
void strip_string (std::string &s);

//!splits a string on the first instance of a specified character
/*!
\param[in] c character to split on
\param[in] s string to split
\param[out] s1 first part of split string
\param[out] s2 second part of split string
*/
void split_string (char c, std::string &s, std::string &s1, std::string &s2);

//!converts an integer into a string
std::string int_to_string (long i);

//------------------------------------------------------------------------------
//writing

//!checks if a file can be written to
void check_file_write (const char *fn);

//!writes an array to a binary file
/*!
\param[in] fn target file path
\param[in] a array of numbers to write
\param[in] size length of array
*/
template <class T>
void write_array (const char *fn, T *a, long size) {
    FILE* ofile;
    check_file_write(fn);
    ofile = fopen(fn, "wb");
    fwrite(a, sizeof(T), size, ofile);
    fclose(ofile);
}

//!writes an array to a binary file
/*!
\param[in] fn target file path
\param[in] a array of numbers to write
\param[in] size length of array
*/
template <class T>
void write_array (const std::string &fn, T *a, long size) {
    write_array(fn.c_str(), a, size);
}

//!writes an array to a binary file
/*!
\param[in] fn target file path
\param[in] a vector of numbers to write
*/
template <class T>
void write_array (const char *fn, std::vector<T> a) {
    write_array(fn, a.data(), long(a.size()));
}

//!writes an array to a binary file
/*!
\param[in] fn target file path
\param[in] a vector of numbers to write
*/
template <class T>
void write_array (const std::string &fn, std::vector<T> a) {
    write_array(fn.c_str(), a.data(), long(a.size()));
}

//------------------------------------------------------------------------------
//reading

//!checks if a file can be opened for reading
/*!
\param[in] fn path to file to check
*/
void check_file_read (const char *fn);

//!reads a settings file into a vector of vectors of strings
/*!
\param[in] fn path to settings file
*/
std::vector< std::vector< std::string > > read_values (const char *fn);

#endif
