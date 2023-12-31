use criterion::{black_box, criterion_group, criterion_main, Criterion};
use std::{ops::Sub, time::Duration};
use utils::{generate_random_elements, to_lambdaworks_vec};

pub mod utils;

const BENCHMARK_NAME: &str = "sub";

pub fn criterion_benchmark(c: &mut Criterion) {
    let arkworks_vec = generate_random_elements(2000000);

    // arkworks-ff
    {
        c.bench_function(
            &format!("{} 1M elements | ark-ff - ef8f758", BENCHMARK_NAME),
            |b| {
                b.iter(|| {
                    let mut iter = arkworks_vec.iter();

                    for _i in 0..1000000 {
                        let a = iter.next().unwrap();
                        let b = iter.next().unwrap();
                        black_box(black_box(&a).sub(black_box(b)));
                    }
                });
            },
        );
    }

    // lambdaworks-math
    {
        let lambdaworks_vec = to_lambdaworks_vec(&arkworks_vec);
        c.bench_function(
            &format!("{} 1M elements | lambdaworks", BENCHMARK_NAME,),
            |b| {
                b.iter(|| {
                    let mut iter = lambdaworks_vec.iter();

                    for _i in 0..1000000 {
                        let a = iter.next().unwrap();
                        let b = iter.next().unwrap();
                        black_box(black_box(&a).sub(black_box(b)));
                    }
                });
            },
        );
    }
}

criterion_group! {
    name = benches;
    config = Criterion::default()
        .significance_level(0.01)
        .measurement_time(Duration::from_secs(15))
        .sample_size(500);
    targets = criterion_benchmark
}
criterion_main!(benches);
