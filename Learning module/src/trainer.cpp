#include "trainer.h"
#include "datasetparameters.h"

Trainer::Trainer() :
    m_neural_network(0)
{

}

void Trainer::init(const NeuralNetworkParameters &neural_network_parameters)
{
    m_neural_network_parameters = neural_network_parameters;
    m_neural_network = new NeuralNetwork(m_neural_network_parameters.m_network_architecture);

    //  Activation functions
    MultilayerPerceptron *multilayer_perceptron = m_neural_network->get_multilayer_perceptron_pointer();
    for(size_t i = 1; i < m_neural_network_parameters.m_layers_activation_functions.size(); ++i)
        multilayer_perceptron->set_layer_activation_function(i, m_neural_network_parameters.m_layers_activation_functions[i]);
}

void Trainer::init(const std::string &neural_network_file_path)
{
    *m_neural_network = loadNeuralNetwork(neural_network_file_path);
}

/*
 * Extract DataSet from a file which path is 'data_set_path'
 * the file must respect the architecture of the network
 *
 * then, train the network with the DataSet
 * */
void Trainer::trainNetwork(DataSet &data_set, DataSetParameters &data_set_parameters)
{
    try
    {
        // Prepare DataSet
        Variables* variables_pointer = data_set.get_variables_pointer();

        size_t i = 0;
        for(; i < m_neural_network_parameters.m_number_of_inputs; ++i)
            variables_pointer->set_use(i, Variables::Input);

        for(; i < m_neural_network_parameters.m_number_of_inputs + m_neural_network_parameters.m_number_of_outputs; ++i)
            variables_pointer->set_use(i, Variables::Target);

        for(i = 0; i < m_neural_network_parameters.m_variables_names.size(); ++i)
            variables_pointer->set_name(i, m_neural_network_parameters.m_variables_names[i]);

        Matrix<std::string> inputs_information = variables_pointer->arrange_inputs_information();
        Matrix<std::string> targets_information = variables_pointer->arrange_targets_information();

        Inputs* inputs_pointer = m_neural_network->get_inputs_pointer();
        inputs_pointer->set_information(inputs_information);

        Outputs* outputs_pointer = m_neural_network->get_outputs_pointer();
        outputs_pointer->set_information(targets_information);

        //  Training stategy + performance goal
        PerformanceFunctional performance_functional(m_neural_network, &data_set);
        TrainingStrategy training_strategy(&performance_functional);

        training_strategy.set_main_type(data_set_parameters.m_training_parameters.m_training_method);

        switch(data_set_parameters.m_training_parameters.m_training_method)
        {
        case TrainingMethod::QUASI_NEWTON_METHOD:
        {
            QuasiNewtonMethod *quasi_Newton_method_pointer = training_strategy.get_quasi_Newton_method_pointer();
            quasi_Newton_method_pointer->set_gradient_norm_goal(data_set_parameters.m_training_parameters.m_gradient_norm_goal);
        }
            break;

        case TrainingMethod::LEVENBERG_MARQUARDT_ALGORITHM:
        {
            LevenbergMarquardtAlgorithm *levenberq_marquardt_pointer = training_strategy.get_Levenberg_Marquardt_algorithm_pointer();
            levenberq_marquardt_pointer->set_gradient_norm_goal(data_set_parameters.m_training_parameters.m_gradient_norm_goal);
        }
            break;

        case TrainingMethod::GRADIENT_DESCENT:
        {
            GradientDescent *gradient_descent_pointer = training_strategy.get_gradient_descent_pointer();
            gradient_descent_pointer->set_gradient_norm_goal(data_set_parameters.m_training_parameters.m_gradient_norm_goal);
        }
            break;

        case TrainingMethod::CONJUGATE_GRADIENT:
        {
            ConjugateGradient *conjugate_gradient_pointer = training_strategy.get_conjugate_gradient_pointer();
            conjugate_gradient_pointer->set_gradient_norm_goal(data_set_parameters.m_training_parameters.m_gradient_norm_goal);
        }
            break;

        case TrainingMethod::NEWTON_METHOD:
        {
            NewtonMethod *newton_method_pointer = training_strategy.get_Newton_method_pointer();
            newton_method_pointer->set_gradient_norm_goal(data_set_parameters.m_training_parameters.m_gradient_norm_goal);
        }
            break;

        default:
            break;
        }

        training_strategy.perform_training();
    }
    catch(std::exception& e)
    {
        std::cerr << "TRAINER::TRAINNETWORK::" << e.what() << std::endl;
    }
}

/*
 * Load and return DataSet from data_set_path
 * The DataSet must respect the architecture
 * of the network
 */
DataSet Trainer::loadDataSet(const std::string &data_set_path)
{
    DataSet data_set;
    data_set.set_data_file_name(data_set_path);
    data_set.load_data();

    Variables* variables_pointer = data_set.get_variables_pointer();

    size_t i = 0;
    for(; i < m_neural_network_parameters.m_number_of_inputs; ++i)
        variables_pointer->set_use(i, Variables::Input);

    for(; i < m_neural_network_parameters.m_number_of_inputs + m_neural_network_parameters.m_number_of_outputs; ++i)
        variables_pointer->set_use(i, Variables::Target);

    for(i = 0; i < m_neural_network_parameters.m_variables_names.size(); ++i)
        variables_pointer->set_name(i, m_neural_network_parameters.m_variables_names[i]);

    Matrix<std::string> inputs_information = variables_pointer->arrange_inputs_information();
    Matrix<std::string> targets_information = variables_pointer->arrange_targets_information();

    Inputs* inputs_pointer = m_neural_network->get_inputs_pointer();
    inputs_pointer->set_information(inputs_information);

    Outputs* outputs_pointer = m_neural_network->get_outputs_pointer();
    outputs_pointer->set_information(targets_information);

    return data_set;
}

/*
 * Load a neural network using
 * saved path
 * */
NeuralNetwork Trainer::loadNeuralNetwork(const std::string &neural_network_path)
{
    try
    {
        NeuralNetwork neural_network;
        neural_network.load(neural_network_path);
        return neural_network;
    }
    catch(std::exception& e)
    {
       std::cerr << "TRAINER::LOADNEURALNETWORK::ERROR" << e.what() << std::endl;

       return NeuralNetwork();
    }
}
