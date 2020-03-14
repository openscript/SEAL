#include "thesis.h"
#include <iostream>
#include <cstdlib>
#include <chrono>

using namespace std;
using namespace seal;

int main(int argc, char** argv) {
    if (argc != 6) {
        cout << argv[0] << " [poly_modulus_degree] [plain_modulus] [iterations] [initial] [add]" << endl;
        return 0;
    }

    print_example_banner(
        "IMPL-01: Verkettung von einfachen Additionen"
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

    int count = atoi(argv[3]);
    int initial = atoi(argv[4]);
    int add = atoi(argv[5]);

    Plaintext initial_plain(to_string(initial));
    Plaintext add_plain(to_string(add));
    Ciphertext add_encrypted;
    Ciphertext current_encrypted;
    encryptor.encrypt(initial_plain, current_encrypted);
    encryptor.encrypt(add_plain, add_encrypted);
    cout << "Initialized with 0x" + initial_plain.to_string() << endl;
    cout << "Initial noise budget:" << decryptor.invariant_noise_budget(current_encrypted) << " bits" << endl;

    Plaintext decrypted_value;
    ofstream outfile;
    outfile.open("impl-01.csv");
    auto t1 = chrono::high_resolution_clock::now();
    
    for( int i = 0; i < count; i = i + 1 ) {
        outfile << i + 1 << ",";
        evaluator.add_inplace(current_encrypted, add_encrypted);
        outfile << decryptor.invariant_noise_budget(current_encrypted);
    }
    auto t2 = chrono::high_resolution_clock::now();

    auto duration = chrono::duration_cast<chrono::microseconds>( t2 - t1 ).count();
    cout << "Duration: " << duration << " microseconds" << endl;
    decryptor.decrypt(current_encrypted, decrypted_value);
    cout << "Result: " << "0x" + decrypted_value.to_string() << endl;

    cout << "Allocated memory: " << MemoryManager::GetPool().alloc_byte_count() << endl;
}
