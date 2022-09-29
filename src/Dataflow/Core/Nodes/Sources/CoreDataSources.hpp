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
    static const std::string getTypename();
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
    static const std::string getTypename();
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
    static const std::string getTypename();
};

/** \brief Partial specialization of SingleDataSourceNode for  for Core::Containers::VectorArray.
 */
template <typename T>
class VectorArrayDataSource : public SingleDataSourceNode<Ra::Core::VectorArray<T>>
{
  protected:
    VectorArrayDataSource( const std::string& instanceName, const std::string& typeName );

  public:
    explicit VectorArrayDataSource( const std::string& name ) :
        VectorArrayDataSource( name, VectorArrayDataSource<T>::getTypename() ) {}

  protected:
    void toJsonInternal( nlohmann::json& data ) const override;
    void fromJsonInternal( const nlohmann::json& data ) override;

  private:
    static const std::string getTypename();
};

} // namespace Sources
} // namespace Core
} // namespace Dataflow
} // namespace Ra

#include <Dataflow/Core/Nodes/Sources/CoreDataSources.inl>
