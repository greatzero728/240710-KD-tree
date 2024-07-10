#include "CS350Loader.hpp"
#include "Utils.hpp"


//std::istream& operator>>(std::istream& is, glm::mat4& matrix) {
//    for (int i = 0; i < 4; ++i) {
//        for (int j = 0; j < 4; ++j) {
//            is >> matrix[i][j];
//        }
//    }
//    return is;
//}

namespace CS350 
{
   // using mat4 = glm::mat4;

    bool ReadBoolean(std::ifstream& input) 
    {
        bool value = false;
        input.read(reinterpret_cast<char*>(&value), sizeof(value));
        return value;
    }

    uint32_t ReadUInt32(std::ifstream& input)
    {
        uint32_t value =0;
        input.read(reinterpret_cast<char*>(&value), sizeof(value));
        return value;
    }

    glm::vec3 ReadVec3(std::ifstream& input) 
    {
        glm::vec3 value(0.0f, 0.0f, 0.0f);
        input.read(reinterpret_cast<char*>(&value), sizeof(value));
        return value;
    }

    glm::vec2 ReadVec2(std::ifstream& input) 
    {
        glm::vec2 value;
        input.read(reinterpret_cast<char*>(&value), sizeof(value));
        return value;
    }

    void VerifySignature(std::ifstream& input, const std::string& file) 
    {
        char signature[5];
        input.read(signature, 5);
        if (std::string(signature, 5) != "CS350") {
            throw std::runtime_error("Invalid file signature in file: " + file);
        }
    }

    void ReadAttributes(std::ifstream& input, bool& hasPositions, bool& hasNormals, bool& hasUVs) 
    {
        hasPositions = ReadBoolean(input);
        hasNormals = ReadBoolean(input);
        hasUVs = ReadBoolean(input);
    }

    void ReadVertices(std::ifstream& input, CS350PrimitiveData& data, uint32_t vertexCount, bool hasPositions, bool hasNormals, bool hasUVs) 
    {
        for (uint32_t i = 0; i < vertexCount; ++i)
        {
            if (hasPositions) 
            {
                data.positions.push_back(ReadVec3(input));
            }
            if (hasNormals) 
            {
                data.normals.push_back(ReadVec3(input));
            }
            if (hasUVs) 
            {
                data.uvs.push_back(ReadVec2(input));
            }
        }
    }

    void ReadPolygons(std::ifstream& input, CS350PrimitiveData& data, uint32_t indexCount) 
    {
        for (uint32_t i = 0; i < indexCount / 3; ++i) {
            CS350PrimitiveData::Face face;
            input.read(reinterpret_cast<char*>(&face), sizeof(face));
            data.polygons.push_back(face);
        }
    }

    void CalculateBoundingVolume(CS350PrimitiveData& data) 
    {
        if (!data.positions.empty()) 
        {
            data.bvMin = data.bvMax = data.positions[0];
            for (const auto& pos : data.positions) 
            {
                data.bvMin = glm::min(data.bvMin, pos);
                data.bvMax = glm::max(data.bvMax, pos);
            }
        }
    }

    CS350PrimitiveData LoadCS350Binary(const std::string& file) 
    {
        CS350PrimitiveData data;
        std::ifstream input(file, std::ios::binary);
        if (!input) 
        {
            throw std::runtime_error("Failed to open file");
        }

        VerifySignature(input, file);

        uint32_t vertexCount = ReadUInt32(input);
        uint32_t indexCount = ReadUInt32(input);

        bool hasPositions = false;
        bool hasNormals =false;
        bool hasUVs = false;
        ReadAttributes(input, hasPositions, hasNormals, hasUVs);

        ReadVertices(input, data, vertexCount, hasPositions, hasNormals, hasUVs);

        if (indexCount != 0) {
            ReadPolygons(input, data, indexCount);
        }

        CalculateBoundingVolume(data);

        return data;
    }

    template <typename T>
    inline std::istream& GenericVecRead(std::istream& is, T& v)
    {
        for (int i = 0; i < T::length(); ++i) {
            is >> v[i];
            if (i + 1 != T::length()) {
                is.ignore(1, ',');
            }
        }
        return is;
    }

    std::vector<CS350SceneObject> LoadCS350Scene(std::string const& file)
    {
        std::vector<CS350SceneObject> sceneObjects{};

        std::ifstream inputFile(file);

        if (!inputFile) {
            throw std::runtime_error("Cannot open file: " + file);
        }

        std::string line;

        while (std::getline(inputFile, line))
        {
            CS350SceneObject sceneObject{};

            // Read the asset index
            std::istringstream indexStream(line);
            indexStream >> sceneObject.primitiveIndex;

            // Read the next line for the m2w matrix
            if (!std::getline(inputFile, line))
            {
                throw std::runtime_error("Unexpected end of file while reading m2w matrix");
            }

            std::istringstream matrixStream(line);
            GenericVecRead(matrixStream, sceneObject.m2w);

            sceneObjects.push_back(sceneObject);
        }

        inputFile.close();
        return sceneObjects;
    }

}
