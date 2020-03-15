#include "thesis.h"
#include <iostream>
#include <cstdlib>
#include <chrono>

using namespace std;
using namespace seal;

int main(int argc, char** argv) {
    if (argc != 6) {
        cout << argv[0] << " [poly_modulus_degree] [plain_modulus] [iterations] [initial] [division]" << endl;
        return 0;
    }

    print_example_banner(
        "IMPL-04: Verkettung von einfachen Divisionen"
    );
    cout << "Allocated memory: " << MemoryManager::GetPool().alloc_byte_count() << endl;


    EncryptionParameters parms(scheme_type::CKKS);

    size_t poly_modulus_degree = atoi(argv[1]);
    parms.set_poly_modulus_degree(poly_modulus_degree);
    parms.set_coeff_modulus(CoeffModulus::Create(
        poly_modulus_degree, { 40, 40, 40, 40, 40 }));

    auto context = SEALContext::Create(parms);
    print_parameters(context);
    cout << endl;

    KeyGenerator keygen(context);
    auto public_key = keygen.public_key();
    auto secret_key = keygen.secret_key();
    auto relin_keys = keygen.relin_keys();

    Encryptor encryptor(context, public_key);
    Evaluator evaluator(context);
    Decryptor decryptor(context, secret_key);

    CKKSEncoder encoder(context);


    size_t slot_count = encoder.slot_count();
    cout << "Number of slots: " << slot_count << endl;

    vector<double> input{ atof(argv[4]) };
    cout << "Input vector: " << endl;
    print_vector(input);

    Plaintext plain;
    double scale = pow(2.0, 5);
    print_line(__LINE__);
    cout << "Encode input vector." << endl;
    encoder.encode(input, scale, plain);

    vector<double> output;
    cout << "    + Decode input vector ...... Correct." << endl;
    encoder.decode(plain, output);
    print_vector(output);


    Ciphertext current_encrypted;
    print_line(__LINE__);
    cout << "Encrypt input vector, square, and relinearize." << endl;
    encryptor.encrypt(plain, current_encrypted);


    Plaintext plain_division;
    encoder.encode(atof(argv[5]), scale, plain_division);

    for( int i = 0; i < atoi(argv[3]); i = i + 1 ) {
        evaluator.multiply_plain_inplace(current_encrypted, plain_division);
        evaluator.relinearize_inplace(current_encrypted, relin_keys);
    }

    cout << "Decrypt and decode." << endl;
    decryptor.decrypt(current_encrypted, plain);
    encoder.decode(plain, output);
    cout << "    + Result vector ...... Correct." << endl;
    print_vector(output);
    cout << output[0] << endl;
}
