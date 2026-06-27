#ifndef __MEM_CACHE_REPLACEMENT_POLICIES_LRU_VARIATION_HH__
#define __MEM_CACHE_REPLACEMENT_POLICIES_LRU_VARIATION_HH__

#include <memory>
#include <vector>

#include "mem/cache/replacement_policies/base.hh"
#include "params/LRU_Variation.hh"

namespace ReplacementPolicy {

class LRU_Variation;

struct LRUVariationReplData : ReplacementData
{
    // Tick representing when this block was last touched
    Tick tickRef;

    // Position (index) within the set
    int position;

    // Pointer to replacement data shared within the set
    std::shared_ptr<std::vector<int>> setPosition;

    LRUVariationReplData(const int numWays)
        : tickRef(0), position(0), 
          setPosition(std::make_shared<std::vector<int>>(numWays)) {}
};

class LRU_Variation : public Base
{
  private:
    const std::vector<int> promotionVector = {0,0,1,0,1,0,1,2,1,0,5,1,0,0,3,11};
    const int insertPosition = 10;  // Based on your configuration
    const unsigned numWays;

  public:
    /** Convenience typedef. */
    typedef LRU_VariationParams Params;

    /** 
     * Construct and initialize this replacement policy.
     */
    LRU_Variation(const Params &p);

    /**
     * Destructor.
     */
    ~LRU_Variation() = default;

    void reset(const std::shared_ptr<ReplacementData>& replacement_data) const override;
    void touch(const std::shared_ptr<ReplacementData>& replacement_data) const override;
    void invalidate(const std::shared_ptr<ReplacementData>& replacement_data) const override;
    ReplaceableEntry* getVictim(const ReplacementCandidates& candidates) const override;
    std::shared_ptr<ReplacementData> instantiateEntry() override;
};

} // namespace ReplacementPolicy

#endif // __MEM_CACHE_REPLACEMENT_POLICIES_LRU_VARIATION_HH__
