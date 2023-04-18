#pragma once
#include <vector>

class ChessNeuralNetwork {
public:
    ChessNeuralNetwork();

    // Value network functions
    float predict_value(std::vector<float>& input);
    std::vector<float> get_value_gradient(std::vector<float>& input, float target);
    void update_value_weights(std::vector<float>& gradient, float learning_rate);

    // Policy network functions
    std::vector<float> predict_policy(std::vector<float>& input);
    std::vector<float> get_policy_gradient(std::vector<float>& input, std::vector<float>& policy_target);
    void update_policy_weights(std::vector<float>& gradient, float learning_rate);

private:
    // Value network
    std::vector<std::vector<float>> value_weights;
    std::vector<float> value_bias1;
    std::vector<float> value_hidden;
    std::vector<float> value_hidden_grad;
    float value_bias2;
    std::vector<float> value_input_grad;

    // Policy network
    std::vector<std::vector<float>> policy_weights;
    std::vector<float> policy_bias1;
    std::vector<float> policy_hidden;
    std::vector<float> policy_hidden_grad;
    std::vector<float> policy_hidden_grad_sum;
    float policy_bias2;

    // Input and output gradients for policy network
    std::vector<float> policy_input_grad;
    std::vector<float> policy_output_grad;
    std::vector<float> policy_output;

    // Vector of binary values indicating whether each possible move is legal
    std::vector<bool> legal_moves;
};