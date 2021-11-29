from uwimg import *
from argparse import ArgumentParser

def evaluate(model, train, test):
    print("evaluating model...")
    print("training accuracy: %f", accuracy_model(model, train))
    print("test accuracy:     %f", accuracy_model(model, test))

def load_mnist():
    print("loading data...")
    train = load_classification_data(b"mnist.train", b"mnist.labels", 1)
    test  = load_classification_data(b"mnist.test",  b"mnist.labels", 1)
    print("done")
    return train, test

def load_cifar10():
    print("loading data...")
    train = load_classification_data(b"cifar.train", b"cifar\labels.txt", 1)
    test  = load_classification_data(b"cifar.test",  b"cifar\labels.txt", 1)
    print("done")
    return train, test

def get_activation(act):
    if act == 'logistic':
        return LOGISTIC
    elif act == 'lrelu':
        return LRELU
    else:
        return RELU

def train_ann(model, train, options):
    print('Training model...')
    train_model(
        model, 
        train, 
        options.batch_size, 
        options.epochs, 
        options.lr,
        options.momentum,
        options.decay,
    )

def mnist(options):
    train, test = load_mnist()
    model = make_model([
        make_layer(train.X.cols, train.y.cols, SOFTMAX)
    ])
    train_ann(model, train, options)
    evaluate(model, train, test)

def mnist_softmax(options):
    train, test = load_mnist()
    act = get_activation(options.act)
    model = make_model([
        make_layer(train.X.cols, 32, act),
        make_layer(32, train.y.cols, SOFTMAX)
    ])
    train_ann(model, train, options)
    evaluate(model, train, test)

def mnist_3_layer(options):
    train, test = load_mnist()
    act = get_activation(options.act)
    model = make_model([
        make_layer(train.X.cols, 64, act),
        make_layer(64, 32, act),
        make_layer(32, train.y.cols, SOFTMAX)
    ])
    train_ann(model, train, options)
    evaluate(model, train, test)

def cifar10(options):
    train, test = load_cifar10()
    act = get_activation(options.act)
    model = make_model([
        make_layer(train.X.cols, 512, act),
        make_layer(512, 256, act),
        make_layer(256, train.y.cols, SOFTMAX),
    ])
    train_ann(model, train, options)
    evaluate(model, train, test)


if __name__ == '__main__':
    parser = ArgumentParser()
    parser.add_argument('--task', type=str, default='mnist')
    parser.add_argument('--batch_size', type=int, default=128)
    parser.add_argument('--epochs', type=int, default=1000)
    parser.add_argument('--lr', type=float, default=.01)
    parser.add_argument('--momentum', type=float, default=.9)
    parser.add_argument('--decay', type=float, default=.0001)
    parser.add_argument('--act', type=str, default='relu')

    args = parser.parse_args()
    if args.task == 'mnist':
        mnist(args)
    elif args.task == 'cifar10':
        cifar10(args)
    elif args.task == 'mnist_softmax':
        mnist_softmax(args)
    elif args.task == 'mnist_3_layer':
        mnist_3_layer(args)
    else:
        print("unknown task: %s" % args.task)


