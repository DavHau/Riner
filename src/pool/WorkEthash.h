
#pragma once

#include <src/pool/Pool.h>
#include <src/pool/Work.h>
#include <src/common/Pointers.h>
#include <src/util/Bytes.h>
#include <src/common/Span.h>
#include <src/algorithm/ethash/DagCache.h>
#include <src/util/DifficultyTarget.h>

namespace riner {

    //see WorkDummy for explanation of the basic concepts of Work/PowType/Solution
    struct HasPowTypeEthash {
        static inline constexpr auto &getPowType() {
            return "ethash";
        }
    };

    class WorkEthash : public Work, public HasPowTypeEthash {
    public:

        WorkEthash() : Work(getPowType()) {
        }

        Bytes<32> header;
        Bytes<32> seedHash;
        Bytes<32> jobTarget; //actual target of the PoolJob
        Bytes<32> deviceTarget; //easier target than above for GPUs

        double jobDifficulty;
        double deviceDifficulty = 60e6; //60 Mh, roughly 2s on a GPU

        uint32_t epoch = std::numeric_limits<uint32_t>::max();
        uint32_t extraNonce;

        /**
         * setEpoch is called on the work templte in the Ethash Pool's workQueue, so that the epoch calculation doesn't happen on the io thread, but the queue's refill thread instead
         */
        void setEpoch() {
            if (epoch == std::numeric_limits<uint32_t>::max()) {
                epoch = calculateEthEpoch(seedHash); //expensive-ish
            }
        }

        /**
         * called to finish initializing by setting difficulty related members
         */
        void setDifficultiesAndTargets(const Bytes<32> &jobTarget) {
            jobDifficulty = targetToDifficultyApprox(jobTarget);
            this->jobTarget = jobTarget;
            deviceDifficulty = std::min(deviceDifficulty, jobDifficulty); //make sure deviceDiff is not harder than jobDiff
            deviceTarget = difficultyToTargetApprox(deviceDifficulty);
        }
    };

    class WorkSolutionEthash : public WorkSolution, public HasPowTypeEthash {
    public:
        using work_type = WorkEthash;

        WorkSolutionEthash(const WorkEthash &work)
                : WorkSolution(static_cast<const Work&>(work))
                , jobDifficulty(work.jobDifficulty) {
        }

        Bytes<32> header;
        Bytes<32> mixHash; // intermediate hash to prevent DOS

        uint64_t nonce = 0;
        double jobDifficulty;
    };

}







