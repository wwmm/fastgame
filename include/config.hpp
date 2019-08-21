#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <vector>

class Config {
 public:
  Config(const std::string& cfg_path);

  template <typename T>
  T get_key(const std::string& key, T default_value) {
    return root.get<T>(key, default_value);
  }

  template <typename T>
  std::vector<T> get_key_array(const std::string& key) {
    std::vector<T> v;

    try {
      for (auto& p : root.get_child(key)) {
        v.push_back(p.second.get<T>(""));
      }
    } catch (const boost::property_tree::ptree_error& e) {
    }

    return v;
  }

 private:
  boost::property_tree::ptree root;
};

#endif