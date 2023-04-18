#include <cmath>
#include "network.h"

ChessNeuralNetwork::ChessNeuralNetwork() {
    // Initialize the weights and biases to small random values
    value_weights.resize(64, std::vector<float>(32));
    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 32; j++) {
            value_weights[i][j] = rand() / (float)RAND_MAX * 0.01f;
        }
    }
    value_bias1.resize(32);
    value_bias2 = rand() / (float)RAND_MAX * 0.01f;
    for (int i = 0; i < 32; i++) {
        value_bias1[i] = rand() / (float)RAND_MAX * 0.01f;
    }
}

float ChessNeuralNetwork::predict_value(std::vector<float>& input) {
    // Perform a forward pass through the value network
    value_hidden.resize(32);
    for (int j = 0; j < 32; j++) {
        float sum = 0.0f;
        for (int i = 0; i < 64; i++) {
            sum += input[i] * value_weights[i][j];
        }
        value_hidden[j] = std::tanh(sum + value_bias1[j]);
    }
    float output = 0.0f;
    for (int j = 0; j < 32; j++) {
        output += value_hidden[j] * value_weights[64][j];
    }
    output += value_bias2;
    return output;
}

std::vector<float> ChessNeuralNetwork::get_value_gradient(std::vector<float>& input, float target) {
    // Compute the gradients of the loss with respect to the weights and biases of the value network
    std::vector<float> gradient(33);
    float output = predict_value(input);
    float error = target - output;
    float output_grad = error;
    for (int j = 0; j < 32; j++) {
        gradient[64 + j] = value_hidden[j] * output_grad;
        value_hidden_grad[j] = output_grad * value_weights[64][j] * (1.0f - value_hidden[j] * value_hidden[j]);
    }
    for (int i = 0; i < 64; i++) {
        float sum = 0.0f;
        for (int j = 0; j < 32; j++) {
            gradient[i * 32 + j] = input[i] * value_hidden_grad[j];
            sum += value_hidden_grad[j] * value_weights[i][j];
        }
        value_input_grad[i] = sum * (1.0f - input[i] * input[i]);
    }
    gradient[2016] = output_grad;
    return gradient;
}

std::vector<float> ChessNeuralNetwork::predict_policy(std::vector<float>& input) {
    // Perform a forward pass through the policy network
    policy_hidden.resize(32);
    for (int j = 0; j < 32; j++) {
        float sum = 0.0f;
        for (int i = 0; i < 64; i++) {
            sum += input[i] * policy_weights[i][j];
        }
        policy_hidden[j] = std::tanh(sum + policy_bias1[j]);
    }
    float output_sum = 0.0f;
    for (int j = 0; j < 32; j++) {
        output_sum += policy_hidden[j] * policy_weights[64][j];
    }
    float output = std::tanh(output_sum + policy_bias2);

    // Convert the output to a probability distribution over legal moves
    std::vector<float> policy_output(4096);
    float total_prob = 0.0f;
    for (int i = 0; i < 4096; i++) {
        policy_output[i] = exp(output * input[i]) * legal_moves[i];
        total_prob += policy_output[i];
    }
    for (int i = 0; i < 4096; i++) {
        policy_output[i] /= total_prob;
    }
    return policy_output;
}

std::vector<float> ChessNeuralNetwork::get_policy_gradient(std::vector<float>& input, std::vector<float>& policy_target) {
    // Compute the gradients of the loss with respect to the weights and biases
    std::vector<float> policy_gradient(33 * 4096);
    float output = predict_value(input);
    float output_grad = 2 * (output - policy_target[4096]);
    for (int j = 0; j < 32; j++) {
        policy_gradient[64 * 4096 + j * 4096 + 4096] = policy_hidden[j] * output_grad;
        policy_hidden_grad[j] = output_grad * policy_weights[64][j] * (1.0f - policy_hidden[j] * policy_hidden[j]);
    }
    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 32; j++) {
            policy_gradient[i * 4096 + j * 4096 + 4096] = input[i] * policy_hidden_grad[j];
            policy_hidden_grad_sum[j] += policy_hidden_grad[j] * policy_weights[i][j];
        }
    }
    for (int i = 0; i < 4096; i++) {
        policy_input_grad[i] = policy_target[i] - policy_output[i];
    }
    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 32; j++) {
            for (int k = 0; k < 4096; k++) {
                policy_gradient[i * 4096 + j * 4096 + k] = policy_input_grad[k] * policy_hidden_grad_sum[j];
            }
        }
    }
    policy_gradient[64 * 4096 + 4096] = output_grad;
    return policy_gradient;
}

void ChessNeuralNetwork::update_policy_weights(std::vector<float>& gradient, float learning_rate) {
    // Update the weights and biases using the computed gradients
    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 32; j++) {
            float delta = 0.0f;
            for (int k = 0; k < 4096; k++) {
                delta += gradient[i * 4096 + j * 4096 + k];
            }
            policy_weights[i][j] += learning_rate * delta;
        }
    }
    for (int j = 0; j < 32; j++) {
        float delta = 0.0f;
        for (int k = 0; k < 4096; k++) {
            delta += gradient[64 * 4096 + j * 4096 + k];
        }
        policy_weights[64][j] += learning_rate * delta;
        policy_bias1[j] += learning_rate * gradient[1984 + j];
    }
    policy_bias2 += learning_rate * gradient[2016];
}
