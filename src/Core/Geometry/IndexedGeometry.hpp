#pragma once

#include <Core/Containers/VectorArray.hpp>
#include <Core/Geometry/TriangleMesh.hpp>
#include <map>

namespace Ra {
namespace Core {
namespace Geometry {

///
/// \brief Base class for index collections stored in MultiIndexedGeometry
/// The name of an IndexView is consistent with its type and semantic.
class RA_CORE_API IndexViewBase : public Utils::ObservableVoid
{
  public:
    inline explicit IndexViewBase( const IndexViewBase& other ) : _viewName( other._viewName ) {}

    inline IndexViewBase( IndexViewBase&& other ) : _viewName( other._viewName ) {}
    inline IndexViewBase& operator=( const IndexViewBase& other ) {
        CORE_ASSERT( _viewName == other._viewName, "Try to assign IndexView of different type" );
        return *this;
    }
    inline IndexViewBase& operator=( IndexViewBase&& other ) {
        CORE_ASSERT( _viewName == other._viewName, "Try to assign IndexView of different type" );
        return *this;
    }

    inline const std::string& viewName() const { return _viewName; }

  protected:
    inline IndexViewBase( const std::string& viewName ) : _viewName( viewName ) {}

  private:
    const std::string _viewName;
};

/// \brief Typed index collection
template <typename T>
struct IndexView : public IndexViewBase {
    using IndexType          = T;
    using IndexContainerType = AlignedStdVector<IndexType>;

    virtual IndexContainerType& collection()             = 0;
    virtual const IndexContainerType& collection() const = 0;
};

/// Simple Mesh structure that handles indexed geometry with vertex
/// attributes. Each face is index collection is stored as IndexView.
class RA_CORE_API MultiIndexedGeometry : public AttribArrayGeometry, public Utils::ObservableVoid
{
  public:
    inline MultiIndexedGeometry() = default;
    explicit MultiIndexedGeometry( const MultiIndexedGeometry& other );
    explicit MultiIndexedGeometry( MultiIndexedGeometry&& other );
    MultiIndexedGeometry& operator=( const MultiIndexedGeometry& other );
    MultiIndexedGeometry& operator=( MultiIndexedGeometry&& other );

    void clear() override;

    /// Copy only the geometry and the indices from \p other, but not the attributes.
    void copy( const MultiIndexedGeometry& other );

    /// Check that the MultiIndexedGeometry is well built, asserting when it is not.
    /// only compiles to something when in debug mode.
    void checkConsistency() const;

    // /// Appends another MultiIndexedGeometry to this one, but only if they have the same
    // attributes.
    // /// Return True if \p other has been successfully appended.
    // /// \warning There is no error check on the handles attribute type.
    // inline bool append( const MultiIndexedGeometry& other );

    /// read only access to indices
    bool indicesExists( const std::string& name ) const;

    /// read only access to indices
    /// \throws std::out_of_range
    const IndexViewBase& getIndices( const std::string& name ) const;

    /// read write access to indices.
    /// Cause indices to be "lock" for the caller
    /// need to be unlock by the caller before any one can ask for write access.
    /// \throws std::out_of_range
    IndexViewBase& getIndicesWithLock( const std::string& name );

    /// unlock previously read write acces, notify observers of the update.
    /// \throws std::out_of_range
    void indicesUnlock( const std::string& name );

    // /// set indices. Indices must be unlock, i.e. no one should have write
    // /// access to it.
    // /// Notify observers of the update.
    // commented: does move operator work with inheritance ?
    // void setIndices( IndexViewBase&& indices );
    void setIndices( const IndexViewBase& indices );

  private:
    // Collection of pairs <lockStatus, Indices>
    std::map<std::string, std::pair<bool, IndexViewBase>> m_indices;
};

} // namespace Geometry
} // namespace Core
} // namespace Ra
