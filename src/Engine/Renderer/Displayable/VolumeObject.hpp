#pragma once

#include <Engine/RaEngine.hpp>
#include <Engine/Renderer/Displayable/DisplayableObject.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Renderer/Texture/Texture.hpp>

namespace Ra {
namespace Core {
namespace Geometry {
class AbstractVolume;
} // namespace Geometry
} // namespace Core
namespace Engine {

/**
 * A class representing an openGL volume
 */
class RA_ENGINE_API VolumeObject : public Displayable {

  public:
    explicit VolumeObject( const std::string& name );
    VolumeObject( const VolumeObject& rhs ) = delete;
    void operator=( const VolumeObject& rhs ) = delete;

    ~VolumeObject() override;

    using Displayable::getName;

    /// Returns the underlying AbstractGeometry, which is in fact a AbstractVolume
    /// \see getTriangleMesh
    inline const Core::Geometry::AbstractGeometry& getGeometry() const override;
    inline Core::Geometry::AbstractGeometry& getGeometry() override;

    /// Returns the underlying AbstractVolume
    inline const Core::Geometry::AbstractVolume& getVolume() const;
    inline Core::Geometry::AbstractVolume& getVolume();

    /// Use the given volume for display. \warning Takes the pointer ownership
    void loadGeometry( Core::Geometry::AbstractVolume* volume );

    /// Mark the data types as dirty, forcing an update of the openGL buffer.
    inline void setDirty();

    /**
     * This function is called at the start of the rendering. It will update the
     * necessary openGL buffers.
     */
    void updateGL() override;

    /// Draw the mesh.
    void render() override;

    /// 6 quad faces of the cube
    size_t getNumFaces() const override { return 12; }
    /// 8 vertices of the cube
    inline size_t getNumVertices() const override { return 8; }

  private:
    std::unique_ptr<Core::Geometry::AbstractVolume> m_volume;
    Texture m_tex;
    /// Mesh used to display the bounding box of the grid for the ray marching
    Mesh m_mesh;

    /// General dirty bit of the mesh. An empty volume is not dirty
    bool m_isDirty{false};
};

} // namespace Engine
} // namespace Ra

#include <Engine/Renderer/Displayable/VolumeObject.inl>
