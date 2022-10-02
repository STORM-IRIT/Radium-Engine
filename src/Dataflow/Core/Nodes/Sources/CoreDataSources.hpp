#pragma once
#include <Dataflow/Core/Nodes/Sources/SingleDataSourceNode.hpp>

#include <Core/Containers/VectorArray.hpp>
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

using FloatArrayDataSource  = SingleDataSourceNode<Ra::Core::VectorArray<float>>;
using DoubleArrayDataSource = SingleDataSourceNode<Ra::Core::VectorArray<double>>;
using IntArrayDataSource    = SingleDataSourceNode<Ra::Core::VectorArray<int>>;
using UIntArrayDataSource   = SingleDataSourceNode<Ra::Core::VectorArray<unsigned int>>;
using ColorArrayDataSource  = SingleDataSourceNode<Ra::Core::VectorArray<Ra::Core::Utils::Color>>;

} // namespace Sources
} // namespace Core
} // namespace Dataflow
} // namespace Ra

#include <Dataflow/Core/Nodes/Sources/CoreDataSources.inl>
