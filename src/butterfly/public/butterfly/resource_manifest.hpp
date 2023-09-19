#ifndef BUTTERFLY_RESOURCE_MANIFEST_HPP
#define BUTTERFLY_RESOURCE_MANIFEST_HPP

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace butterfly {
    class resource_manifest {
    public:
        void update( const std::string& serialized_manifest );
        std::string resource_lookup( uint64_t hash ) const;

    private:
        struct path_parts {
            uint32_t dir_id;
            uint32_t file_name_id;
            uint32_t type_id;
        };

        std::vector<std::string> directories;
        std::vector<std::string> types;
        std::vector<std::string> file_names;
        std::unordered_map<uint64_t, path_parts> path_hash_to_path_parts;
    };
}  // namespace butterfly

#endif /* BUTTERFLY_RESOURCE_MANIFEST_HPP */
