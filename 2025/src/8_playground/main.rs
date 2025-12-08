use std::{
    cmp::Ordering,
    fs::File,
    io::Read,
    num::ParseIntError,
    ops::{Add, AddAssign, Mul, Sub, SubAssign},
    str::FromStr,
};

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
struct Point {
    x: i64,
    y: i64,
    z: i64,
}

impl FromStr for Point {
    type Err = ParseIntError;

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        let parts = s.trim().split(",").collect::<Vec<&str>>();

        Ok(Point {
            x: parts[0].parse::<i64>()?,
            y: parts[1].parse::<i64>()?,
            z: parts[2].parse::<i64>()?,
        })
    }
}

impl Point {
    fn norm(&self) -> f32 {
        ((*self * *self) as f32).sqrt()
    }

    fn distance(&self, other: &Point) -> f32 {
        (*self - *other).norm()
    }
}

impl Add for Point {
    type Output = Point;

    fn add(self, rhs: Self) -> Self::Output {
        Point {
            x: self.x + rhs.x,
            y: self.y + rhs.y,
            z: self.z + rhs.z,
        }
    }
}

impl AddAssign for Point {
    fn add_assign(&mut self, rhs: Self) {
        *self = Point {
            x: self.x + rhs.x,
            y: self.y + rhs.y,
            z: self.z + rhs.z,
        }
    }
}

impl Sub for Point {
    type Output = Point;

    fn sub(self, rhs: Self) -> Self::Output {
        Point {
            x: self.x - rhs.x,
            y: self.y - rhs.y,
            z: self.z - rhs.z,
        }
    }
}

impl SubAssign for Point {
    fn sub_assign(&mut self, rhs: Self) {
        *self = Point {
            x: self.x - rhs.x,
            y: self.y - rhs.y,
            z: self.z - rhs.z,
        }
    }
}

impl Mul for Point {
    type Output = i64;

    fn mul(self, rhs: Self) -> Self::Output {
        self.x * rhs.x + self.y * rhs.y + self.z * rhs.z
    }
}

#[derive(Debug, Clone, Copy)]
struct Connection {
    p1: Point,
    p2: Point,
}

impl Connection {
    fn distance(&self) -> f32 {
        self.p1.distance(&self.p2)
    }
}

struct Playground {
    junction_boxes: Vec<Point>,
}

impl From<String> for Playground {
    fn from(value: String) -> Self {
        let lines = value.trim().split("\r\n");

        Playground {
            junction_boxes: lines.filter_map(|line| line.parse().ok()).collect(),
        }
    }
}

impl Playground {
    fn remove_pos_from_vec(vec: &mut Vec<Point>, p: &Point) {
        if let Some(index) = Self::find_index(vec, p) {
            vec.remove(index);
        }
    }

    fn find_index(vec: &Vec<Point>, p: &Point) -> Option<usize> {
        vec.iter()
            .enumerate()
            .find_map(|(index, pos)| (*p == *pos).then(|| index))
    }

    fn find_circuit_index(vec: &Vec<Vec<Point>>, p: &Point) -> Option<usize> {
        vec.iter()
            .enumerate()
            .find_map(|(index, circuit)| circuit.contains(p).then(|| index))
    }

    fn create_connection(
        circuits: &mut Vec<Vec<Point>>,
        conn: &Connection,
        unconnected_junction_boxes: &mut Vec<Point>,
    ) {
        let p1_found = Self::find_circuit_index(&circuits, &conn.p1);
        let p2_found = Self::find_circuit_index(&circuits, &conn.p2);

        match (p1_found, p2_found) {
            (Some(c1), None) => {
                circuits[c1].push(conn.p2);
                Self::remove_pos_from_vec(unconnected_junction_boxes, &conn.p2);
            }
            (None, Some(c2)) => {
                circuits[c2].push(conn.p1);
                Self::remove_pos_from_vec(unconnected_junction_boxes, &conn.p1);
            }
            (Some(c1), Some(c2)) => match c1.cmp(&c2) {
                Ordering::Less => {
                    let (left, right) = circuits.split_at_mut(c2);
                    left[c1].append(&mut right[0]);

                    circuits.remove(c2);
                }
                Ordering::Greater => {
                    let (left, right) = circuits.split_at_mut(c1);
                    left[c2].append(&mut right[0]);

                    circuits.remove(c1);
                }
                _ => {}
            },
            _ => {
                circuits.push(vec![conn.p1, conn.p2]);
                Self::remove_pos_from_vec(unconnected_junction_boxes, &conn.p1);
                Self::remove_pos_from_vec(unconnected_junction_boxes, &conn.p2);
            }
        }
    }

    fn connect_boxes(&self, connections_count: u32) -> Vec<Vec<Point>> {
        let mut connections = Vec::<Connection>::new();
        connections
            .reserve((self.junction_boxes.len() * (self.junction_boxes.len() - 1) / 2) as usize);

        for i in 0..self.junction_boxes.len() {
            for j in i + 1..self.junction_boxes.len() {
                connections.push(Connection {
                    p1: self.junction_boxes[i],
                    p2: self.junction_boxes[j],
                });
            }
        }

        connections.sort_by(|c1, c2| match (c1.distance(), c2.distance()) {
            (d1, d2) if d1 < d2 => Ordering::Less,
            (d1, d2) if d1 > d2 => Ordering::Greater,
            _ => Ordering::Equal,
        });

        let mut unconnected_junction_boxes = self.junction_boxes.clone();
        let mut result = connections.iter().take(connections_count as usize).fold(
            Vec::<Vec<Point>>::new(),
            |mut circuits, conn| {
                Self::create_connection(&mut circuits, conn, &mut unconnected_junction_boxes);
                circuits
            },
        );

        result.extend(
            unconnected_junction_boxes
                .iter()
                .map(|&junction_box| vec![junction_box]),
        );

        result
    }

    fn connect_all_boxes(&self) -> Option<Connection> {
        let mut connections = Vec::<Connection>::new();
        connections
            .reserve((self.junction_boxes.len() * (self.junction_boxes.len() - 1) / 2) as usize);

        for i in 0..self.junction_boxes.len() {
            for j in i + 1..self.junction_boxes.len() {
                connections.push(Connection {
                    p1: self.junction_boxes[i],
                    p2: self.junction_boxes[j],
                });
            }
        }

        connections.sort_by(|c1, c2| match (c1.distance(), c2.distance()) {
            (d1, d2) if d1 < d2 => Ordering::Less,
            (d1, d2) if d1 > d2 => Ordering::Greater,
            _ => Ordering::Equal,
        });

        let mut connections_iter = connections.iter();
        let mut circuits = Vec::<Vec<Point>>::new();
        let mut unconnected_junction_boxes = self.junction_boxes.clone();
        let mut last_conn: Option<Connection> = None;

        while !(unconnected_junction_boxes.is_empty() && circuits.len() == 1)
            && let Some(conn) = connections_iter.next()
        {
            Self::create_connection(&mut circuits, conn, &mut unconnected_junction_boxes);
            last_conn = Some(*conn);
        }

        last_conn
    }
}

fn main() -> std::io::Result<()> {
    // let input = "
    //     162,817,812\r\n
    //     57,618,57\r\n
    //     906,360,560\r\n
    //     592,479,940\r\n
    //     352,342,300\r\n
    //     466,668,158\r\n
    //     542,29,236\r\n
    //     431,825,988\r\n
    //     739,650,466\r\n
    //     52,470,668\r\n
    //     216,146,977\r\n
    //     819,987,18\r\n
    //     117,168,530\r\n
    //     805,96,715\r\n
    //     346,949,466\r\n
    //     970,615,88\r\n
    //     941,993,340\r\n
    //     862,61,35\r\n
    //     984,92,344\r\n
    //     425,690,689\r\n
    //     "
    // .to_string();
    let mut file = File::open("src/8_playground/data.txt")?;
    let mut input = String::new();
    let _ = file.read_to_string(&mut input);

    let playground = Playground::from(input);
    let mut circuits = playground.connect_boxes(1000);
    circuits.sort_by(|c1, c2| c2.len().cmp(&c1.len()));

    println!("{:?} circuits count", circuits.len());

    println!(
        "Product of the sizes of the tree largest circuits: {}",
        circuits
            .iter()
            .take(3)
            .map(|circuit| circuit.len())
            .product::<usize>()
    );

    if let Some(last_connection) = playground.connect_all_boxes() {
        println!("Last connection: {:?}", last_connection);
        println!(
            "Product of x values: {}",
            last_connection.p1.x * last_connection.p2.x
        );
    }

    Ok(())
}
