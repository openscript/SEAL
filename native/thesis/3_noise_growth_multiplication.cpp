#include "thesis.h"
#include <iostream>
#include <cstdlib>
#include <chrono>

using namespace std;
using namespace seal;

int main(int argc, char** argv) {
    if (argc != 6) {
        cout << argv[0] << " [poly_modulus_degree] [plain_modulus] [iterations] [initial] [multi]" << endl;
        return 0;
    }

    print_example_banner(
        "IMPL-03: Verkettung von einfachen Multiplikationen"
    );
    cout << "Allocated memory: " << MemoryManager::GetPool().alloc_byte_count() << endl;

    EncryptionParameters parms(scheme_type::BFV);

    size_t poly_modulus_degree = atoi(argv[1]);
    parms.set_poly_modulus_degree(poly_modulus_degree);
    parms.set_coeff_modulus(
        CoeffModulus::BFVDefault(poly_modulus_degree)
    );
    parms.set_plain_modulus(atoi(argv[2]));

    auto context = SEALContext::Create(parms);

    print_line(__LINE__);
    cout << "Set encryption parameters and print" << endl;
    print_parameters(context);
    cout << endl;

    KeyGenerator keygen(context);
    PublicKey public_key = keygen.public_key();
    SecretKey secret_key = keygen.secret_key();

    Encryptor encryptor(context, public_key);
    Evaluator evaluator(context);
    Decryptor decryptor(context, secret_key);

    int iterations = atoi(argv[3]);

    stringstream initial_stream;
    initial_stream << hex << atoi(argv[4]);

    stringstream multi_stream;
    multi_stream << hex << atoi(argv[5]);

    Plaintext initial_plain(initial_stream.str());
    Plaintext multi_plain(multi_stream.str());
    Ciphertext multi_encrypted;
    Ciphertext current_encrypted;
    encryptor.encrypt(initial_plain, current_encrypted);
    encryptor.encrypt(multi_plain, multi_encrypted);
    cout << "Initialized with 0x" + initial_plain.to_string() << endl;
    cout << "Initial noise budget:" << decryptor.invariant_noise_budget(current_encrypted) << " bits" << endl;

    Plaintext decrypted_value;
    ofstream outfile;
    stringstream filename;
    filename << "impl-03-" << argv[3] << ".csv";
    outfile.open(filename.str());
    
    auto t1 = chrono::high_resolution_clock::now();
    for( int i = 0; i < iterations; i = i + 1 ) {
        outfile << i + 1 << ",";
        evaluator.multiply_inplace(current_encrypted, multi_encrypted);
        outfile << decryptor.invariant_noise_budget(current_encrypted) << endl;
    }
    auto t2 = chrono::high_resolution_clock::now();

    auto duration = chrono::duration_cast<chrono::microseconds>( t2 - t1 ).count();
    cout << "Duration: " << duration << " microseconds" << endl;
    decryptor.decrypt(current_encrypted, decrypted_value);

    stringstream result_stream;
    result_stream << decrypted_value.to_string();
    int result;
    result_stream >> hex >> result;
    cout << "Result: " << result << endl;
    cout << "Noise budget:" << decryptor.invariant_noise_budget(current_encrypted) << " bits" << endl;
    cout << "Allocated memory: " << MemoryManager::GetPool().alloc_byte_count() << endl;
}
