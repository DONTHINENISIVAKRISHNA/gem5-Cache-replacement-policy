#include "mem/cache/replacement_policies/lru_variation.hh"

#include <cassert>
#include <memory>

#include "base/logging.hh"
#include "sim/core.hh"
#include "params/LRU_Variation.hh"

namespace ReplacementPolicy {

LRU_Variation::LRU_Variation(const Params &p)
    : Base(p), numWays(16)  // Fixed size for this implementation
{
}

void
LRU_Variation::reset(const std::shared_ptr<ReplacementData>& replacement_data) const
{
    std::shared_ptr<LRUVariationReplData> repl_data =
        std::static_pointer_cast<LRUVariationReplData>(replacement_data);

    repl_data->tickRef = curTick();

    int oldPos = repl_data->setPosition->at(repl_data->position);
    for (int i = 0; i < numWays; i++) {
        if ((*repl_data->setPosition)[i] < oldPos) {
            (*repl_data->setPosition)[i]++;
        }
    }
    (*repl_data->setPosition)[repl_data->position] = insertPosition;
}

void
LRU_Variation::touch(const std::shared_ptr<ReplacementData>& replacement_data) const
{
    std::shared_ptr<LRUVariationReplData> repl_data =
        std::static_pointer_cast<LRUVariationReplData>(replacement_data);

    repl_data->tickRef = curTick();

    int currentPos = repl_data->setPosition->at(repl_data->position);
    int newPos = (currentPos < promotionVector.size()) ? promotionVector[currentPos] : 0;

    for (int i = 0; i < numWays; i++) {
        if ((*repl_data->setPosition)[i] < currentPos &&
            (*repl_data->setPosition)[i] >= newPos) {
            (*repl_data->setPosition)[i]++;
        }
    }
    (*repl_data->setPosition)[repl_data->position] = newPos;
}

void
LRU_Variation::invalidate(const std::shared_ptr<ReplacementData>& replacement_data) const
{
    std::shared_ptr<LRUVariationReplData> repl_data =
        std::static_pointer_cast<LRUVariationReplData>(replacement_data);

    int oldPos = repl_data->setPosition->at(repl_data->position);
    for (int i = 0; i < numWays; i++) {
        if ((*repl_data->setPosition)[i] > oldPos) {
            (*repl_data->setPosition)[i]--;
        }
    }
    (*repl_data->setPosition)[repl_data->position] = numWays - 1;
}

ReplaceableEntry*
LRU_Variation::getVictim(const ReplacementCandidates& candidates) const
{
    assert(candidates.size() > 0);

    ReplaceableEntry* victim = candidates[0];
    int maxPosition = std::static_pointer_cast<LRUVariationReplData>(
        victim->replacementData)->setPosition->at(
        std::static_pointer_cast<LRUVariationReplData>(
        victim->replacementData)->position);

    for (const auto& candidate : candidates) {
        std::shared_ptr<LRUVariationReplData> candidate_data =
            std::static_pointer_cast<LRUVariationReplData>(candidate->replacementData);
        
        int candidatePosition = candidate_data->setPosition->at(candidate_data->position);
        if (candidatePosition > maxPosition) {
            victim = candidate;
            maxPosition = candidatePosition;
        }
    }

    return victim;
}

std::shared_ptr<ReplacementData>
LRU_Variation::instantiateEntry()
{
    std::shared_ptr<LRUVariationReplData> replData =
        std::make_shared<LRUVariationReplData>(numWays);
    
    static int currentPosition = 0;
    replData->position = currentPosition;
    
    if (currentPosition == 0) {
        for (int i = 0; i < numWays; i++) {
            (*replData->setPosition)[i] = i;
        }
    }
    
    currentPosition = (currentPosition + 1) % numWays;
    
    return replData;
}

} // namespace ReplacementPolicy
