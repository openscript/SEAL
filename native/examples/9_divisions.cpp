#include "examples.h"

using namespace std;
using namespace seal;

void divisions() {
    print_example_banner("Division");
    /*
    [CKKSEncoder] (For CKKS scheme only)

    In this example we demonstrate the Cheon-Kim-Kim-Song (CKKS) scheme for
    computing on encrypted real or complex numbers. We start by creating
    encryption parameters for the CKKS scheme. There are two important
    differences compared to the BFV scheme:

        (1) CKKS does not use the plain_modulus encryption parameter;
        (2) Selecting the coeff_modulus in a specific way can be very important
            when using the CKKS scheme. We will explain this further in the file
            `ckks_basics.cpp'. In this example we use CoeffModulus::Create to
            generate 5 40-bit prime numbers.
    */
    EncryptionParameters parms(scheme_type::CKKS);

    size_t poly_modulus_degree = 8192;
    parms.set_poly_modulus_degree(poly_modulus_degree);
    parms.set_coeff_modulus(CoeffModulus::Create(
        poly_modulus_degree, { 40, 40, 40, 40, 40 }));

    /*
    We create the SEALContext as usual and print the parameters.
    */
    auto context = SEALContext::Create(parms);
    print_parameters(context);
    cout << endl;

    /*
    Keys are created the same way as for the BFV scheme.
    */
    KeyGenerator keygen(context);
    auto public_key = keygen.public_key();
    auto secret_key = keygen.secret_key();
    auto relin_keys = keygen.relin_keys();

    /*
    We also set up an Encryptor, Evaluator, and Decryptor as usual.
    */
    Encryptor encryptor(context, public_key);
    Evaluator evaluator(context);
    Decryptor decryptor(context, secret_key);

    /*
    To create CKKS plaintexts we need a special encoder: there is no other way
    to create them. The IntegerEncoder and BatchEncoder cannot be used with the
    CKKS scheme. The CKKSEncoder encodes vectors of real or complex numbers into
    Plaintext objects, which can subsequently be encrypted. At a high level this
    looks a lot like what BatchEncoder does for the BFV scheme, but the theory
    behind it is completely different.
    */
    CKKSEncoder encoder(context);

    /*
    In CKKS the number of slots is poly_modulus_degree / 2 and each slot encodes
    one real or complex number. This should be contrasted with BatchEncoder in
    the BFV scheme, where the number of slots is equal to poly_modulus_degree
    and they are arranged into a matrix with two rows.
    */
    size_t slot_count = encoder.slot_count();
    cout << "Number of slots: " << slot_count << endl;

    /*
    We create a small vector to encode; the CKKSEncoder will implicitly pad it
    with zeros to full size (poly_modulus_degree / 2) when encoding.
    */
    vector<double> input{ 1.0, 2.0, 3.0, 20.0 };
    cout << "Input vector: " << endl;
    print_vector(input);

    /*
    Now we encode it with CKKSEncoder. The floating-point coefficients of `input'
    will be scaled up by the parameter `scale'. This is necessary since even in
    the CKKS scheme the plaintext elements are fundamentally polynomials with
    integer coefficients. It is instructive to think of the scale as determining
    the bit-precision of the encoding; naturally it will affect the precision of
    the result.

    In CKKS the message is stored modulo coeff_modulus (in BFV it is stored modulo
    plain_modulus), so the scaled message must not get too close to the total size
    of coeff_modulus. In this case our coeff_modulus is quite large (200 bits) so
    we have little to worry about in this regard. For this simple example a 30-bit
    scale is more than enough.
    */
    Plaintext plain;
    double scale = pow(2.0, 30);
    print_line(__LINE__);
    cout << "Encode input vector." << endl;
    encoder.encode(input, scale, plain);

    /*
    We can instantly decode to check the correctness of encoding.
    */
    vector<double> output;
    cout << "    + Decode input vector ...... Correct." << endl;
    encoder.decode(plain, output);
    print_vector(output);

    /*
    The vector is encrypted the same was as in BFV.
    */
    Ciphertext encrypted;
    print_line(__LINE__);
    cout << "Encrypt input vector, square, and relinearize." << endl;
    encryptor.encrypt(plain, encrypted);

    /*
    Basic operations on the ciphertexts are still easy to do. Here we square the
    ciphertext, decrypt, decode, and print the result. We note also that decoding
    returns a vector of full size (poly_modulus_degree / 2); this is because of
    the implicit zero-padding mentioned above.
    */
    Plaintext plain_half;
    encoder.encode(0.123123123, scale, plain_half);
    evaluator.multiply_plain_inplace(encrypted, plain_half);
    evaluator.relinearize_inplace(encrypted, relin_keys);

    /*
    We notice that the scale in the result has increased. In fact, it is now the
    square of the original scale: 2^60.
    */
    cout << "    + Scale in squared input: " << encrypted.scale()
        << " (" << log2(encrypted.scale()) << " bits)" << endl;

    print_line(__LINE__);
    cout << "Decrypt and decode." << endl;
    decryptor.decrypt(encrypted, plain);
    encoder.decode(plain, output);
    cout << "    + Result vector ...... Correct." << endl;
    print_vector(output);
    cout << output[0] << endl;

    /*
    The CKKS scheme allows the scale to be reduced between encrypted computations.
    This is a fundamental and critical feature that makes CKKS very powerful and
    flexible. We will discuss it in great detail in `3_levels.cpp' and later in
    `4_ckks_basics.cpp'.
    */
}
