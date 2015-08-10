#include <Engine/Renderer/FancyMeshPlugin/FancyMeshLoader.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <Core/Log/Log.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Mesh/TriangleMesh.hpp>
#include <Core/Mesh/MeshUtils.hpp>
#include <Core/String/StringUtils.hpp>
#include <Engine/RadiumEngine.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/Renderer/RenderTechnique/Material.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgramManager.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Renderer/Light/DirLight.hpp>
#include <Engine/Renderer/Light/SpotLight.hpp>
#include <Engine/Renderer/Light/PointLight.hpp>

namespace Ra
{

    namespace
    {
        const Engine::ShaderConfiguration defaultShaderConf( "BlinnPhong", "../Shaders" );

        std::string filepath;
        static DataVector dataVector;

        void assimpToCore( const aiVector3D& inVector, Core::Vector3& outVector );
        void assimpToCore( const aiColor4D& inColor, Core::Color& outColor );
        void assimpToCore( const aiMatrix4x4& inMatrix, Core::Matrix4& outMatrix );

        Core::Vector3 assimpToCore( const aiVector3D&  vector );
        Core::Color   assimpToCore( const aiColor4D&   color );
        Core::Matrix4 assimpToCore( const aiMatrix4x4& matrix );

        void runThroughNodes( const aiNode* node, const aiScene* scene,
                              const Core::Matrix4& transform );

        void loadMesh( const aiMesh* mesh, Engine::FancyMeshData& data );

        void loadRenderTechnique( const aiMaterial* mat, Engine::FancyComponentData& data );
        void loadDefaultRenderTechnique( Engine::FancyComponentData& data );
    }

    DataVector Engine::FancyMeshLoader::loadFile( const std::string& name )
    {
        dataVector.clear();

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile( name,
                                                  aiProcess_Triangulate |
                                                  aiProcess_JoinIdenticalVertices |
                                                  aiProcess_GenSmoothNormals |
                                                  aiProcess_SortByPType |
                                                  aiProcess_FixInfacingNormals |
                                                  aiProcess_CalcTangentSpace |
                                                  aiProcess_GenUVCoords );
        if ( !scene )
        {
            //        LOG(ERROR) << "Error while loading file \"" << name << "\" : " << importer.GetErrorString() << ".";
            return dataVector;
        }

        //    LOG(DEBUG) << "About to load file " << name;
        //    LOG(DEBUG) << "Found " << scene->mNumMeshes << " meshes and " << scene->mNumMaterials << " materials.";

        filepath = Core::StringUtils::getDirName( name );

        runThroughNodes( scene->mRootNode, scene, Core::Matrix4::Identity() );

        //LOG(DEBUG) << "File " << name << " loaded successfully (" << dataVector.size() << " items to load).";

        return dataVector;
    }

    namespace
    {

        void runThroughNodes( const aiNode* node, const aiScene* scene,
                              const Core::Matrix4& transform )
        {
            if ( node->mNumChildren == 0 && node->mNumMeshes == 0 )
            {
                return;
            }

            Core::Matrix4 matrix = transform * assimpToCore( node->mTransformation );

            if ( node->mNumMeshes > 0 )
            {
                Engine::FancyComponentData data;
                data.transform = matrix;
                data.name = node->mName.C_Str();

                // Consider only the first material for a given component
                if ( scene->HasMaterials() )
                {
                    aiMaterial* material = scene->mMaterials[scene->mMeshes[node->mMeshes[0]]->mMaterialIndex];
                    loadRenderTechnique( material, data );
                }
                else
                {
                    loadDefaultRenderTechnique( data );
                }

                for ( uint i = 0; i < node->mNumMeshes; ++i )
                {
                    Engine::FancyMeshData meshData;

                    aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
                    loadMesh( mesh, meshData );

                    data.meshes.push_back( meshData );
                }

                dataVector.push_back( data );
            }

            for ( uint i = 0; i < node->mNumChildren; ++i )
            {
                runThroughNodes( node->mChildren[i], scene, matrix );
            }
        }

        void loadMesh( const aiMesh* mesh, Engine::FancyMeshData& data )
        {
            Core::Vector3Array positions;
            Core::Vector3Array normals;
            Core::Vector3Array tangents;
            Core::Vector3Array bitangents;
            Core::Vector3Array texcoords;
            std::vector<uint>  indices;

            for ( uint i = 0; i < mesh->mNumVertices; ++i )
            {
                positions.push_back( assimpToCore( mesh->mVertices[i] ) );
                positions.push_back( assimpToCore( mesh->mNormals[i] ) );

                if ( mesh->HasTangentsAndBitangents() )
                {
                    tangents.push_back( assimpToCore( mesh->mTangents[i] ) );
                    bitangents.push_back( assimpToCore( mesh->mBitangents[i] ) );
                }

                // FIXME(Charly): What do texture coords indices mean ?
                if ( mesh->HasTextureCoords( 0 ) )
                {
                    texcoords.push_back( assimpToCore( mesh->mTextureCoords[0][i] ) );
                }
            }

            for ( uint i = 0; i < mesh->mNumFaces; ++i )
            {
                aiFace f = mesh->mFaces[i];

                indices.push_back( f.mIndices[0] );
                indices.push_back( f.mIndices[1] );
                indices.push_back( f.mIndices[2] );
            }

            data.positions = positions;
            data.normals = normals;
            data.tangents = tangents;
            data.bitangents = bitangents;
            data.texcoords = texcoords;
            data.indices = indices;
        }

        void loadRenderTechnique( const aiMaterial* mat, Engine::FancyComponentData& data )
        {
            std::string materialName = data.name.append( "_Material" );
            if ( mat == nullptr )
            {
                loadDefaultRenderTechnique( data );
                return;
            }

            // TODO(Charly): Handle different shader programs
            // TODO(Charly): Handle transparency
            Engine::Material* material = new Engine::Material( materialName );

            aiColor4D color;
            if ( AI_SUCCESS == mat->Get( AI_MATKEY_COLOR_DIFFUSE, color ) )
            {
                Core::Color c = assimpToCore( color );
                material->setKd( c );
                if ( c.w() < 1.0 )
                {
                    material->setMaterialType( Engine::Material::MaterialType::MAT_TRANSPARENT );
                }
            }
            else
            {
                material->setKd( Core::Color( 0, 0, 0, 1 ) );
            }

            if ( AI_SUCCESS == mat->Get( AI_MATKEY_COLOR_SPECULAR, color ) )
            {
                material->setKs( assimpToCore( color ) );
            }
            else
            {
                material->setKs( Core::Color( 0, 0, 0, 1 ) );
            }

            Scalar shininess;
            if ( AI_SUCCESS == mat->Get( AI_MATKEY_SHININESS, shininess ) )
            {
                material->setNs( shininess );
            }
            else
            {
                material->setKs( Core::Color( 0, 0, 0, 1 ) );
                material->setNs( 1.0 );
            }

            aiString name;
            if ( AI_SUCCESS == mat->Get( AI_MATKEY_TEXTURE( aiTextureType_DIFFUSE, 0 ), name ) )
            {
                material->addTexture( Engine::Material::TextureType::TEX_DIFFUSE,
                                      filepath + "/" + std::string( name.C_Str() ) );
            }

            if ( AI_SUCCESS == mat->Get( AI_MATKEY_TEXTURE( aiTextureType_SPECULAR, 0 ), name ) )
            {
                material->addTexture( Engine::Material::TextureType::TEX_SPECULAR,
                                      filepath + "/" + std::string( name.C_Str() ) );
            }

            if ( AI_SUCCESS == mat->Get( AI_MATKEY_TEXTURE( aiTextureType_NORMALS, 0 ), name ) )
            {
                material->addTexture( Engine::Material::TextureType::TEX_NORMAL,
                                      filepath + "/" + std::string( name.C_Str() ) );
            }

            if ( AI_SUCCESS == mat->Get( AI_MATKEY_TEXTURE( aiTextureType_SHININESS, 0 ), name ) )
            {
                material->addTexture( Engine::Material::TextureType::TEX_SHININESS,
                                      filepath + "/" + std::string( name.C_Str() ) );
            }

            if ( AI_SUCCESS == mat->Get( AI_MATKEY_TEXTURE( aiTextureType_OPACITY, 0 ), name ) )
            {
                material->addTexture( Engine::Material::TextureType::TEX_ALPHA,
                                      filepath + "/" + std::string( name.C_Str() ) );
                material->setMaterialType( Engine::Material::MaterialType::MAT_TRANSPARENT );
            }

            Engine::RenderTechnique* renderTechnique = new Engine::RenderTechnique;
            renderTechnique->shaderConfig = defaultShaderConf;
            renderTechnique->material = material;

            data.renderTechnique = renderTechnique;
        }

        void loadDefaultRenderTechnique( Engine::FancyComponentData& data )
        {
            std::string materialName = data.name.append( "_Material" );

            Engine::Material* material = new Engine::Material( materialName );

            Engine::RenderTechnique* renderTechnique = new Engine::RenderTechnique;
            renderTechnique->shaderConfig = defaultShaderConf;
            renderTechnique->material = material;

            data.renderTechnique = renderTechnique;
        }

        void assimpToCore( const aiVector3D& inVector, Core::Vector3& outVector )
        {
            for ( uint i = 0; i < 3; ++i )
            {
                outVector[i] = inVector[i];
            }
        }

        void assimpToCore( const aiColor4D& inColor, Core::Color& outColor )
        {
            for ( uint i = 0; i < 4; ++i )
            {
                outColor[i] = inColor[i];
            }
        }

        void assimpToCore( const aiMatrix4x4& inMatrix, Core::Matrix4& outMatrix )
        {
            for ( uint i = 0; i < 4; ++i )
            {
                for ( uint j = 0; j < 4; ++j )
                {
                    outMatrix( i, j ) = inMatrix[i][j];
                }
            }
        }

        Core::Vector3 assimpToCore( const aiVector3D& vector )
        {
            Core::Vector3 v;
            assimpToCore( vector, v );
            return v;
        }

        Core::Color assimpToCore( const aiColor4D& color )
        {
            Core::Color c;
            assimpToCore( color, c );
            return c;
        }

        Core::Matrix4 assimpToCore( const aiMatrix4x4& matrix )
        {
            Core::Matrix4 m;
            assimpToCore( matrix, m );
            return m;
        }

    } // namespace


} // namespace Ra
