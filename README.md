    /**
 * Locality-Sensitive Hashing Scheme Based on K-means Double-Bit Quantization.
 *
 *
 * For more information on Double-Bit Quantization based LSH, see the following reference.
 *
 *     Zhu, H. (2014). K-means based double-bit quantization for hashing.
 *     Computational Intelligence for Multimedia, Signal and Vision Processing (CIMSIVP),
 *     2014 IEEE Symposium on (pp.1-5). IEEE.
 *
 */
            KDBQ,
    /**
 * Locality-Sensitive Hashing Scheme Based on Iterative Quantization.
 *
 *
 * For more information on iterative quantization based LSH, see the following reference.
 *
 *     Gong Y, Lazebnik S, Gordo A, et al. Iterative quantization: A procrustean
 *     approach to learning binary codes for large-scale image retrieval[J].
 *     Pattern Analysis and Machine Intelligence, IEEE Transactions on, 2013,
 *     35(12): 2916-2929.
 */
    ITQ,
    /**
 * Locality-Sensitive Hashing Scheme Based on Double-Bit Quantization.
 *
 *
 * For more information on Double-Bit Quantization based LSH, see the following reference.
 *
 *     Kong W, Li W. Double-Bit Quantization for Hashing. In AAAI, 2012.
 *
 *     Gong Y, Lazebnik S, Gordo A, et al. Iterative quantization: A procrustean
 *     approach to learning binary codes for large-scale image retrieval[J].
 *     Pattern Analysis and Machine Intelligence, IEEE Transactions on, 2013,
 *     35(12): 2916-2929.
 */
    DBQ,
    /**
 * Locality-Sensitive Hashing Scheme Based on p-Stable Distributions.
 *
 *
 * For more information on p-stable distribution based LSH, see the following reference.
 *
 *     Mayur Datar , Nicole Immorlica , Piotr Indyk , Vahab S. Mirrokni,
 *     Locality-sensitive hashing scheme based on p-stable distributions,
 *     Proceedings of the twentieth annual symposium on Computational geometry, June
 *     08-11, 2004, Brooklyn, New York, USA.
 */
    PSD,
    /**
 * Locality-Sensitive Hashing Scheme Based on Random Bits Sampling.
 *
 *
 * For more information on random bits sampling based LSH, see the following reference.
 *
 *     P. Indyk and R. Motwani. Approximate Nearest Neighbor - Towards Removing
 *     the Curse of Dimensionality. In Proceedings of the 30th Symposium on Theory
 *     of Computing, 1998, pp. 604-613.
 *
 *     A. Gionis, P. Indyk, and R. Motwani. Similarity search in high dimensions
 *     via hashing. Proceedings of the 25th International Conference on Very Large
 *     Data Bases (VLDB), 1999.
 */
    RBS,
/**
 * Locality-Sensitive Hashing Scheme Based on Random Hyperplane.
 *
 *
 * For more information on random hyperplane based LSH, see the following reference.
 *
 *     Charikar, M. S. 2002. Similarity estimation techniques from rounding
 *     algorithms. In Proceedings of the Thiry-Fourth Annual ACM Symposium on
 *     theory of Computing (Montreal, Quebec, Canada, May 19 - 21, 2002). STOC '02.
 *     ACM, New York, NY, 380-388. DOI= http://doi.acm.org/10.1145/509907.509965
 */
    RHP,
    /**
 * Locality-Sensitive Hashing Scheme Based on Spectral Hashing.
 *
 *
 * For more information on spectral hashing based LSH, see the following reference.
 *
 *     Y. Weiss, A. Torralba, R. Fergus. Spectral Hashing.
 *     Advances in Neural Information Processing Systems, 2008.
 */
    SH,
    /**
 * Locality-Sensitive Hashing Scheme Based on Thresholding.
 *
 *
 * For more information on thresholding based LSH, see the following reference.
 *
 *     Zhe Wang, Wei Dong, William Josephson, Qin Lv, Moses Charikar, Kai Li.
 *     Sizing Sketches: A Rank-Based Analysis for Similarity Search. In
 *     Proceedings of the 2007 ACM SIGMETRICS International Conference on
 *     Measurement and Modeling of Computer Systems . San Diego, CA, USA. June
 *     2007.
 *
 *     Qin Lv, Moses Charikar, Kai Li. Image Similarity Search with Compact
 *     Data Structures. In Proceedings of ACM 13th Conference on Information
 *     and Knowledge Management (CIKM), Washington D.C., USA. November 2004.
 */
    TH