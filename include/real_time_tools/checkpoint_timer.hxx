/**
 * @file
 * @license BSD 3-clause
 * @copyright Copyright (c) 2020, New York University and Max Planck
 *            Gesellschaft
 *
 * @brief Implementation of the CheckpointTimer class.
 */

template <size_t NUM_CHECKPOINTS, bool ENABLED>
CheckpointTimer<NUM_CHECKPOINTS, ENABLED>::CheckpointTimer()
{
    static_assert(NUM_CHECKPOINTS > 0,
                  "CheckpointTimer needs at least one checkpoint");
    checkpoint_names_[0] = "Total";
}

template <size_t NUM_CHECKPOINTS, bool ENABLED>
void CheckpointTimer<NUM_CHECKPOINTS, ENABLED>::start()
{
    if (ENABLED)
    {
        timers_[0].tac_tic();
        current_checkpoint_ = 1;
        timers_[current_checkpoint_].tic();
    }
}

template <size_t NUM_CHECKPOINTS, bool ENABLED>
void CheckpointTimer<NUM_CHECKPOINTS, ENABLED>::checkpoint(
    const std::string &checkpoint_name)
{
    if (ENABLED)
    {
        timers_[current_checkpoint_].tac();

        if (checkpoint_names_[current_checkpoint_].empty())
        {
            checkpoint_names_[current_checkpoint_] = checkpoint_name;
        }
        else if (checkpoint_names_[current_checkpoint_] != checkpoint_name)
        {
            throw std::runtime_error("Wrong checkpoint called (expected '" +
                                     checkpoint_names_[current_checkpoint_] +
                                     "' but got '" + checkpoint_name + "').");
        }

        current_checkpoint_++;
        if (current_checkpoint_ < timers_.size())
        {
            timers_[current_checkpoint_].tic();
        }
    }
}

template <size_t NUM_CHECKPOINTS, bool ENABLED>
void CheckpointTimer<NUM_CHECKPOINTS, ENABLED>::print_statistics() const
{
    if (ENABLED)
    {
        std::cout << "======================================" << std::endl;
        for (size_t i = 0; i < timers_.size(); i++)
        {
            std::cout << "===== " << checkpoint_names_[i] << std::endl;
            timers_[i].print_statistics();
        }
    }
}
