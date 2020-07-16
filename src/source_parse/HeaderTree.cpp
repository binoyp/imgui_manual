#include "HeaderTree.h"

namespace SourceParse
{

HeaderTree * searchParent(HeaderTree* headerTree, int siblingHeaderLevel)
{
    HeaderTree *possibleSibling = headerTree;
    while (possibleSibling->value_.level > siblingHeaderLevel)
    {
        if (possibleSibling->parent_ != nullptr)
            possibleSibling = possibleSibling->parent_;
        else
            break;
    }
    assert(possibleSibling != nullptr);

    HeaderTree *parent = possibleSibling->parent_;
    if (possibleSibling->value_.level < siblingHeaderLevel)
        parent = possibleSibling;

    return parent;
}

void makeHeaderTrees_Impl(
    std::deque<LineWithTag>& headerLinesStream,
    HeaderTree& outHeaderTreeParent
)
{
    if (headerLinesStream.empty())
        return;

    LineWithTag nextHeader = headerLinesStream.front();
    headerLinesStream.pop_front();

    HeaderTree *parent = nullptr;
    // Search for correct parent
    {
        if (nextHeader.level > outHeaderTreeParent.value_.level)
        {
            // Append child
            parent = &outHeaderTreeParent;
        }
        else if (nextHeader.level == outHeaderTreeParent.value_.level)
        {
            // Create a new sibling tree
            parent = outHeaderTreeParent.parent_;
        }
        else // if (nextHeader.level < outHeaderTreeParent.value_.level)
        {
            // Search for parent by going up in the parents
            parent = searchParent(&outHeaderTreeParent, nextHeader.level);
        }
    }

    assert(parent != nullptr);
    HeaderTree newTree({nextHeader, {}, parent});
    parent->children_.push_back(newTree);
    HeaderTree& newTreeRef = parent->children_.back();
    makeHeaderTrees_Impl(headerLinesStream, newTreeRef);
}

HeaderTree makeHeaderTree(const LinesWithTags& linesWithTags, const LineWithTag& treeTopLeaf)
{
    std::deque<LineWithTag> headerLinesStream;
    for (const auto& v: linesWithTags)
        headerLinesStream.push_back(v);

    HeaderTree  treeTop { treeTopLeaf, {}, nullptr};
    makeHeaderTrees_Impl(headerLinesStream, treeTop);

    return  treeTop;
}

} // namespace SourceParse
