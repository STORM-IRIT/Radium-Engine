#pragma once
#include <Dataflow/Core/Nodes/Sources/SingleDataSourceNode.hpp>

#include <Core/Containers/VectorArray.hpp>
#include <Core/Types.hpp>
#include <Core/Utils/Color.hpp>

#include <iostream>
#include <utility>

namespace Ra {
namespace Dataflow {
namespace Core {
namespace Sources {
/**
 * Specialization of SingleDataSourceNode for boolean value
 */
class RA_DATAFLOW_API BooleanValueSource : public SingleDataSourceNode<bool>
{
  public:
    explicit BooleanValueSource( const std::string& name );

  protected:
    void toJsonInternal( nlohmann::json& data ) const override;
    void fromJsonInternal( const nlohmann::json& data ) override;

  public:
    static const std::string& getTypename();
};

/**
 * Specialization of SingleDataSourceNode for int value
 */
class RA_DATAFLOW_API IntValueSource : public SingleDataSourceNode<int>
{
  public:
    explicit IntValueSource( const std::string& name );

  protected:
    void toJsonInternal( nlohmann::json& data ) const override;
    void fromJsonInternal( const nlohmann::json& data ) override;

  public:
    static const std::string& getTypename();
};

/**
 * Specialization of SingleDataSourceNode for unsigned int value
 */
class RA_DATAFLOW_API UIntValueSource : public SingleDataSourceNode<unsigned int>
{
  public:
    explicit UIntValueSource( const std::string& name );

  protected:
    void toJsonInternal( nlohmann::json& data ) const override;
    void fromJsonInternal( const nlohmann::json& data ) override;

  public:
    static const std::string& getTypename();
};

/**
 * Specialization of SingleDataSourceNode for scalar value
 */
class RA_DATAFLOW_API ScalarValueSource : public SingleDataSourceNode<Scalar>
{
  public:
    explicit ScalarValueSource( const std::string& name );

  protected:
    void toJsonInternal( nlohmann::json& data ) const override;
    void fromJsonInternal( const nlohmann::json& data ) override;

  public:
    static const std::string& getTypename();
};

/**
 * Specialization of SingleDataSourceNode for Core::Utils::Color value
 */
class RA_DATAFLOW_API ColorSourceNode : public SingleDataSourceNode<Ra::Core::Utils::Color>
{
  public:
    explicit ColorSourceNode( const std::string& name );

  protected:
    void toJsonInternal( nlohmann::json& data ) const override;
    void fromJsonInternal( const nlohmann::json& data ) override;

  public:
    static const std::string& getTypename();
};

// This macro does not end with semicolon. To be added when callin it
#define DECLARE_ARRAYSOURCES( SUFFIX, TYPE ) \
    using ArrayDataSource##SUFFIX = SingleDataSourceNode<Ra::Core::VectorArray<TYPE>>

using namespace Ra::Core;

DECLARE_ARRAYSOURCES( Float, float );
DECLARE_ARRAYSOURCES( Double, double );
DECLARE_ARRAYSOURCES( Int, int );
DECLARE_ARRAYSOURCES( UInt, unsigned int );
DECLARE_ARRAYSOURCES( Color, Utils::Color );
DECLARE_ARRAYSOURCES( Vector2f, Vector2f );
DECLARE_ARRAYSOURCES( Vector2d, Vector2d );
DECLARE_ARRAYSOURCES( Vector3f, Vector3f );
DECLARE_ARRAYSOURCES( Vector3d, Vector3d );
DECLARE_ARRAYSOURCES( Vector4f, Vector4f );
DECLARE_ARRAYSOURCES( Vector4d, Vector4d );
DECLARE_ARRAYSOURCES( Vector2i, Vector2i );
DECLARE_ARRAYSOURCES( Vector3i, Vector3i );
DECLARE_ARRAYSOURCES( Vector4i, Vector4i );
DECLARE_ARRAYSOURCES( Vector2ui, Vector2ui );
DECLARE_ARRAYSOURCES( Vector3ui, Vector3ui );
DECLARE_ARRAYSOURCES( Vector4ui, Vector4ui );

#undef DECLARE_ARRAYSOURCES

} // namespace Sources
} // namespace Core
} // namespace Dataflow
} // namespace Ra
