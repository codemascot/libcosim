/**
 *  \file
 *  Slave interface.
 */
#ifndef CSE_SLAVE_HPP
#define CSE_SLAVE_HPP

#include <string>
#include <gsl/span>
#include <cse/model.hpp>

namespace cse
{


/**
 *  An interface for classes that represent slave instances.
 *
 *  The function call sequence is as follows:
 *
 *    1. `setup()`:
 *          Configure the slave and enter initialisation mode.
 *    2. `set_<type>()`, `get_<type>()`:
 *          Variable initialisation.  The functions may be called multiple times
 *          in any order.
 *    3. `start_simulation()`:
 *          End initialisation mode, start simulation.
 *    4. `do_step()`, `get_<type>()`, `set_<type>()`:
 *          Simulation.  The functions may be called multiple times, in this
 *          order.
 *    5. `end_simulation()`:
 *          End simulation.
 *
 *  Any method may throw an exception, after which the slave instance is considered
 *  to be "broken" and no further method calls will be made.
 */
class slave_instance
{
public:
    virtual ~slave_instance() { }

    /// Returns an object that describes the slave type.
    virtual cse::model_description model_description() const = 0;

    /**
     *  Instructs the slave to perform pre-simulation setup and enter
     *  initialisation mode.
     *
     *  This function is called when the slave has been added to an execution.
     *  The arguments `startTime` and `stopTime` represent the time interval
     *  inside which the slave's model equations are required to be valid.
     *  (In other words, it is guaranteed that do_step() will never be called
     *  with a time point outside this interval.)
     *
     *  \param [in] slaveName
     *      The name of the slave in the current execution.  May be empty.
     *  \param [in] executionName
     *      The name of the current execution.  May be empty.
     *  \param [in] startTime
     *      The earliest possible time point for the simulation.
     *  \param [in] stopTime
     *      The latest possible time point for the simulation.  May be
     *      `eternity`if there is no defined stop time.
     *  \param [in] adaptiveStepSize
     *      Whether the step size is being controlled by error estimation.
     *  \param [in] relativeTolerance
     *      Only used if `adaptiveStepSize == true`, and then contains the
     *      relative tolerance of the step size controller.  The slave may
     *      then use this for error estimation in its internal integrator.
     */
    virtual void setup(
        std::string_view slaveName,
        std::string_view executionName,
        time_point startTime,
        time_point stopTime,
        bool adaptiveStepSize,
        double relativeTolerance) = 0;

    /**
     *  Informs the slave that the initialisation stage ends and the
     *          simulation begins.
     */
    virtual void start_simulation() = 0;

    /// Informs the slave that the simulation run has ended.
    virtual void end_simulation() = 0;

    /**
     *  Performs model calculations for the time step which starts at
     *  the time point `currentT` and has a duration of `deltaT`.
     *
     *  If this is not the first time step, it can be assumed that the previous
     *  time step ended at `currentT`.  It can also be assumed that `currentT`
     *  is greater than or equal to the start time, and `currentT+deltaT` is
     *  less than or equal to the stop time, specified in the setup() call.
     *
     *  \returns
     *      `true` if the model calculations for the given time step were
     *      successfully carried out, or `false` if they were not and the
     *      operation may be retried with a shorter time step.
     *      (Non-recoverable problems must be signaled with an exception.)
     *
     *  \note
     *      Currently, retrying a failed time step is not supported, but this is
     *      planned for a future version.
    */
    virtual bool do_step(time_point currentT, time_duration deltaT) = 0;

    /**
     *  Retrieves the values of real variables.
     *
     *  On return, the `values` array will be filled with the values of the
     *  variables specified in `variables`, in the same order.
     *
     *  \pre `variables.size() == values.size()`
     */
    virtual void get_real_variables(
        gsl::span<const variable_index> variables,
        gsl::span<double> values) const = 0;

    /**
     *  Retrieves the values of integer variables.
     *
     *  On return, the `values` array will be filled with the values of the
     *  variables specified in `variables`, in the same order.
     *
     *  \pre `variables.size() == values.size()`
     */
    virtual void get_integer_variables(
        gsl::span<const variable_index> variables,
        gsl::span<int> values) const = 0;

    /**
     *  Retrieves the values of boolean variables.
     *
     *  On return, the `values` array will be filled with the values of the
     *  variables specified in `variables`, in the same order.
     *
     *  \pre `variables.size() == values.size()`
     */
    virtual void get_boolean_variables(
        gsl::span<const variable_index> variables,
        gsl::span<bool> values) const = 0;

    /**
     *  Retrieves the values of string variables.
     *
     *  On return, the `values` array will be filled with the values of the
     *  variables specified in `variables`, in the same order.
     *
     *  \pre `variables.size() == values.size()`
     */
    virtual void get_string_variables(
        gsl::span<const variable_index> variables,
        gsl::span<std::string> values) const = 0;

    /**
     *  Sets the values of real variables.
     *
     *  This will set the value of each variable specified in the `variables`
     *  array to the value given in the corresponding element of `values`.
     *
     *  \returns
     *      `true` if successful and `false` in the case of non-fatal problems
     *      (e.g. if one or more values were invalid and have simply been
     *      ignored).
     *
     *  \pre `variables.size() == values.size()`
     */
    virtual bool set_real_variables(
        gsl::span<const variable_index> variables,
        gsl::span<const double> values) = 0;

    /**
     *  Sets the values of integer variables.
     *
     *  This will set the value of each variable specified in the `variables`
     *  array to the value given in the corresponding element of `values`.
     *
     *  \returns
     *      `true` if successful and `false` in the case of non-fatal problems
     *      (e.g. if one or more values were invalid and have simply been
     *      ignored).
     *
     *  \pre `variables.size() == values.size()`
     */
    virtual bool set_integer_variables(
        gsl::span<const variable_index> variables,
        gsl::span<const int> values) = 0;

    /**
     *  Sets the values of boolean variables.
     *
     *  This will set the value of each variable specified in the `variables`
     *  array to the value given in the corresponding element of `values`.
     *
     *  \returns
     *      `true` if successful and `false` in the case of non-fatal problems
     *      (e.g. if one or more values were invalid and have simply been
     *      ignored).
     *
     *  \pre `variables.size() == values.size()`
     */
    virtual bool set_boolean_variables(
        gsl::span<const variable_index> variables,
        gsl::span<const bool> values) = 0;

    /**
     *  Sets the values of string variables.
     *
     *  This will set the value of each variable specified in the `variables`
     *  array to the value given in the corresponding element of `values`.
     *
     *  \returns
     *      `true` if successful and `false` in the case of non-fatal problems
     *      (e.g. if one or more values were invalid and have simply been
     *      ignored).
     *
     *  \pre `variables.size() == values.size()`
     */
    virtual bool set_string_variables(
        gsl::span<const variable_index> variables,
        gsl::span<const std::string> values) = 0;
};


}
#endif // header guard
