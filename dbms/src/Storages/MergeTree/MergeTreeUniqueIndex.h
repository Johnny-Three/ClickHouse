#pragma once

#include <Storages/MergeTree/MergeTreeIndices.h>
#include <Storages/MergeTree/MergeTreeData.h>
#include <Storages/MergeTree/KeyCondition.h>

#include <memory>


namespace DB
{

class MergeTreeUniqueIndex;

struct MergeTreeUniqueGranule : public MergeTreeIndexGranule
{
    explicit MergeTreeUniqueGranule(const MergeTreeUniqueIndex & index);

    void serializeBinary(WriteBuffer & ostr) const override;
    void deserializeBinary(ReadBuffer & istr) override;

    String toString() const override;
    bool empty() const override { return !block.rows(); }

    void update(const Block & block, size_t * pos, size_t limit) override;

    ~MergeTreeUniqueGranule() override = default;

    const MergeTreeUniqueIndex & index;
    Block block;
};

class UniqueCondition : public IndexCondition
{
public:
    UniqueCondition(
            const SelectQueryInfo & query,
            const Context & context,
            const MergeTreeUniqueIndex & index);

    bool alwaysUnknownOrTrue() const override;

    bool mayBeTrueOnGranule(MergeTreeIndexGranulePtr idx_granule) const override;

    ~UniqueCondition() override = default;
private:
    const MergeTreeUniqueIndex & index;
    KeyCondition condition;
};


class MergeTreeUniqueIndex : public MergeTreeIndex
{
public:
    MergeTreeUniqueIndex(
            String name,
            ExpressionActionsPtr expr,
            const Names & columns,
            const DataTypes & data_types,
            size_t granularity)
            : MergeTreeIndex(name, expr, columns, data_types, granularity) {}

    ~MergeTreeUniqueIndex() override = default;

    MergeTreeIndexGranulePtr createIndexGranule() const override;

    IndexConditionPtr createIndexCondition(
            const SelectQueryInfo & query, const Context & context) const override;

};

std::unique_ptr<MergeTreeIndex> MergeTreeUniqueIndexCreator(
        const MergeTreeData & data, std::shared_ptr<ASTIndexDeclaration> node, const Context & context);

}