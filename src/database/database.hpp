#pragma once

#include <sqlite3.h>
#include <string>

class Database
{
  public:
    explicit Database(const std::string &path);

    sqlite3 *openConnection() const;
    void initialize();

  private:
    std::string db_path_;
};