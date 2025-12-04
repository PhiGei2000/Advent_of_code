use std::{fs::File, io::Read};

fn is_invalid(id: &String) -> bool {
    match id.chars().nth(0) {
        Some('0') => true,
        Some(_) => {
            let length = id.len();
            // match length % 2 {
            //     1 => false,
            //     0 => {
            
            // let parts = id.split_at(length / 2);
            // parts.0 == parts.1

            for pattern_size in (1..length).rev() {
                if length % pattern_size == 0 {
                    let parts = id
                        .as_bytes()
                        .chunks(pattern_size)
                        .map(|buf| str::from_utf8(buf).unwrap())
                        .collect::<Vec<&str>>();

                    if parts.iter().skip(1).all(|&part| part == parts[0]) {
                        return true;
                    }
                }
            }

            false
        }
        _ => unreachable!(),
    }
    // }
    // None => false,
    // }
}

fn check_range(start: i64, end: i64) -> Vec<i64> {
    let numbers: Vec<i64> = (start..(end + 1)).collect();

    numbers
        .iter()
        .filter(|&id| is_invalid(&id.to_string()))
        .map(|&id| id)
        .collect()
}

fn main() {
    let mut file = File::open("src/2_gift_shop/data.txt").unwrap();
    let mut input = String::new();
    let _ = file.read_to_string(&mut input);

    // let input = "11-22,95-115,998-1012,1188511880-1188511890,222220-222224,1698522-1698528,446443-446449,38593856-38593862,565653-565659,824824821-824824827,2121212118-2121212124";

    let id_ranges = input.trim().split(",").collect::<Vec<&str>>();
    let invalid_ids = id_ranges
        .iter()
        .map(|range| range.split_once("-").unwrap())
        .map(|(start, end)| {
            check_range(
                start
                    .parse::<i64>()
                    .inspect_err(|e| println!("{e} {start}"))
                    .unwrap(),
                end.parse::<i64>()
                    .inspect_err(|e| println!("{e} {end}"))
                    .unwrap(),
            )
        })
        .flatten()
        .collect::<Vec<i64>>();

    println!("{:?}", invalid_ids);

    let sum = invalid_ids.iter().sum::<i64>();

    println!("Sum of invalid ids: {sum}");
}
