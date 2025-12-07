use std::{
    fs::File,
    io::Read,
    ops::{Add, AddAssign},
};

#[derive(Copy, Clone, Debug)]
struct IdRange {
    start: u64,
    end: u64,
}

impl IdRange {
    fn contains(&self, id: u64) -> bool {
        self.start <= id && self.end >= id
    }

    fn count(&self) -> u64 {
        self.end - self.start + 1_u64
    }

    fn overlapping(&self, other: &IdRange) -> bool {
        self.end >= other.start && self.end <= other.end
            || other.end >= self.start && other.end <= self.end
    }
}

impl Add for IdRange {
    type Output = Option<IdRange>;

    fn add(self, rhs: Self) -> Self::Output {
        if self.overlapping(&rhs) {
            Some(IdRange {
                start: self.start.min(rhs.start),
                end: self.end.max(rhs.end),
            })
        } else {
            None
        }
    }
}

impl AddAssign for IdRange {
    fn add_assign(&mut self, rhs: Self) {
        if !self.overlapping(&rhs) {
            panic!();
        }

        *self = Self {
            start: self.start.min(rhs.start),
            end: self.end.max(rhs.end),
        }
    }
}

struct Inventory {
    fresh_ingredients: Vec<IdRange>,
    available_ingredient_ids: Vec<u64>,
}

impl From<String> for Inventory {
    fn from(value: String) -> Self {
        let mut lines = value.split("\r\n");

        let mut fresh_ingredients: Vec<IdRange> = vec![];
        let mut available_ingredient_ids: Vec<u64> = vec![];

        let mut available_ingredients = false;

        while let Some(line) = lines.next() {
            if line.is_empty() {
                available_ingredients = true;
                continue;
            }

            if available_ingredients {
                available_ingredient_ids.push(line.parse::<u64>().unwrap());
            } else {
                if let Some((start, end)) = line.split_once("-") {
                    let id_range = IdRange {
                        start: start.parse::<u64>().unwrap(),
                        end: end.parse::<u64>().unwrap(),
                    };

                    fresh_ingredients.push(id_range);
                }
            }
        }

        // merge fresh ingredients
        let mut i = 0;
        while i < fresh_ingredients.len() {
            let mut merged = false;
            let mut j = i + 1;
            while j < fresh_ingredients.len() {
                if fresh_ingredients[i].overlapping(&fresh_ingredients[j]) {
                    let other = fresh_ingredients[j];
                    fresh_ingredients[i] += other;
                    fresh_ingredients.remove(j);
                    merged = true
                } else {
                    j += 1;
                }
            }

            if merged {
                i = 0;
            } else {
                i += 1;
            }
        }

        Inventory {
            fresh_ingredients,
            available_ingredient_ids,
        }
    }
}

impl Inventory {
    fn is_fresh(&self, id: u64) -> bool {
        match id {
            id if self.available_ingredient_ids.contains(&id) => self
                .fresh_ingredients
                .iter()
                .any(|range| range.contains(id)),
            _ => false,
        }
    }
}

fn main() -> std::io::Result<()> {
    // let input = "3-5\r\n10-14\r\n16-20\r\n12-18\r\n\r\n1\r\n5\r\n8\r\n11\r\n17\r\n32".to_string();
    let mut file = File::open("src/5_cafeteria/data.txt")?;
    let mut input = String::new();
    let _ = file.read_to_string(&mut input);

    let inventory = Inventory::from(input);
    let fresh_ingredients_count = inventory
        .available_ingredient_ids
        .iter()
        .map(|&id| inventory.is_fresh(id))
        .filter(|&res| res)
        .count();

    println!("{fresh_ingredients_count} fresh ingredients");

    // part 2
    let total_fresh_ingredients_count: u64 = inventory
        .fresh_ingredients
        .iter()
        .map(|range| range.count())
        .sum();

    println!("{total_fresh_ingredients_count} fresh ingredients from ranges");

    Ok(())
}
