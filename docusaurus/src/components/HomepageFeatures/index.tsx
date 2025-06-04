import type { ReactNode } from "react";
import clsx from "clsx";
import Heading from "@theme/Heading";
import styles from "./styles.module.css";

type FeatureItem = {
  title: string;
  Svg: React.ComponentType<React.ComponentProps<"svg">>;
  description: ReactNode;
};

const FeatureList: FeatureItem[] = [
  {
    title: "Modular & Portable",
    Svg: require("@site/static/img/modular_portable.svg").default,
    description: (
      <>
        Can be used on Linux, Windows, macOS, Qt, or ESP32 with zero external
        deps
      </>
    ),
  },
  {
    title: "Secure by Design",
    Svg: require("@site/static/img/secure_by_design.svg").default,
    description: (
      <>
        Every connection is validated with an encrypted handshake and encrypted
        session
      </>
    ),
  },
  {
    title: "Lightweight Messaging",
    Svg: require("@site/static/img/lightweight_messaging.svg").default,
    description: <>Faster and smaller than TLS for embedded environments</>,
  },
];

function Feature({ title, Svg, description }: FeatureItem) {
  return (
    <div className={clsx("col col--4")}>
      <div className="text--center">
        <Svg className={styles.featureSvg} role="img" />
      </div>
      <div className="text--center padding-horiz--md">
        <Heading as="h3">{title}</Heading>
        <p>{description}</p>
      </div>
    </div>
  );
}

export default function HomepageFeatures(): ReactNode {
  return (
    <section className={styles.features}>
      <div className="container">
        <div className="row">
          {FeatureList.map((props, idx) => (
            <Feature key={idx} {...props} />
          ))}
        </div>
      </div>
    </section>
  );
}
