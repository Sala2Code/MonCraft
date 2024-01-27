#include "common/include.hpp"
#include "common/TextureArray.hpp"

#include <fstream>

#include "include/constants.hpp"

#include "include/json.hpp"


void loadArrTexture(TextureArray &textureArray){
    std::ifstream ifs(pathJson.c_str());
    nlohmann::json_abi_v3_11_2::json jf;
    if (ifs.is_open()){
        jf = nlohmann::json_abi_v3_11_2::json::parse(ifs);
    }else{
        std::cout << "The texture file could not be opened." << std::endl;
    }
    
    std::string texture_path;
    GLuint count = 0;
    for (auto it = jf.begin(); it != jf.end(); it++){
        for (auto el : jf.items()){
            for (const auto& id : el.value()["id"]) {
                if (id == count) {

                    texture_path = el.value()["texture"];

                    // TODO: transform int (index of texture) to a new struct to store (isTransparent & isFoliage)
                    // ID modulo 3: 0 = Up, 1 = Side, 2 = Down
                    textureArray.AddTextureToArray(texture_path.c_str());

                    count++;
                }
            }
        }
    }
}